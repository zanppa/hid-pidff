// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Force feedback driver for USB HID PID compliant devices
 *
 *  Copyright (c) 2005, 2006 Anssi Hannula <anssi.hannula@gmail.com>
 *                2014 Lauri Peltonen <lauri.peltonen@gmail.com>
 */

/*
 */

#define DEBUG

/* Enable this to debug driver managed memory pool allocations */
#define DEBUG_MEM_ALLOC

/* Enable this to debug scaling of parameters */
#define DEBUG_SCALING


#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/input.h>
#include <linux/slab.h>
#include <linux/usb.h>

#include <linux/hid.h>

#include <linux/list.h>

#include "usbhid.h"

#define IS_DEVICE_MANAGED(device) (test_bit(PID_SUPPORTS_DEVICE_MANAGED, \
			&device->flags))

#define	PID_EFFECTS_MAX		64
/* Only 2 axes are currently supported in the driver.
 * Some code sections would support more,
 * but in some sections 2 is still a hard coded limit.
 */
#define PID_AXES_MAX		2

/* Report usage table used to put reports into an array */

#define PID_SET_EFFECT		0
#define PID_EFFECT_OPERATION	1
#define PID_DEVICE_GAIN		2
#define PID_POOL		3
#define PID_DEVICE_CONTROL	4

#define PID_BLOCK_LOAD		5
#define PID_BLOCK_FREE		6
#define PID_CREATE_NEW_EFFECT	7

#define PID_POOL_MOVE		8

#define PID_SET_ENVELOPE	9
#define PID_SET_CONDITION	10
#define PID_SET_PERIODIC	11
#define PID_SET_CONSTANT	12
#define PID_SET_RAMP		13

#define PID_REQUIRED_REPORTS		4
#define PID_REQUIRED_DEVICE_MANAGED	7
#define PID_REQUIRED_POOL_MOVE		8
static const u8 pidff_reports[] = {
	0x21, 0x77, 0x7d, 0x7f, 0x96,	/* Required for all devices */
	0x89, 0x90, 0xab,		/* Required for device managed */
	0x85,				/* Required for defragmentation */
	0x5a, 0x5f, 0x6e, 0x73, 0x74	/* Others */
};
/* device_control is really 0x95, but 0x96 specified as it is the usage of
 *the only field in that report
 */

/* Value usage tables used to put fields and values into arrays */

#define PID_EFFECT_BLOCK_INDEX		0
#define PID_PARAM_BLOCK_OFFSET		0

#define PID_INDEX_PLACEHOLDER		0xff
#define PID_EFFECT_BLOCK_INDEX_CODE	0x22
#define PID_PARAM_BLOCK_OFFSET_CODE	0x23

#define PID_DURATION			1
#define PID_TRIGGER_BUTTON		2
#define PID_TRIGGER_REPEAT_INT		3
#define PID_DIRECTION_ENABLE		4
#define PID_START_DELAY			5
static const u8 pidff_set_effect[] = {
	0x22, 0x50, 0x53, 0x54, 0x56, 0xa7
};

#define PID_GAIN		0
static const u8 pidff_set_effect_optional[] = {	0x52 };

#define PID_ATTACK_LEVEL	1
#define PID_ATTACK_TIME		2
#define PID_FADE_LEVEL		3
#define PID_FADE_TIME		4
static const u8 pidff_set_envelope[] = {
	PID_INDEX_PLACEHOLDER, 0x5b, 0x5c, 0x5d, 0x5e
};


#define PID_CP_OFFSET		1
#define PID_POS_COEFFICIENT	2
#define PID_NEG_COEFFICIENT	3
#define PID_POS_SATURATION	4
#define PID_NEG_SATURATION	5
#define PID_DEAD_BAND		6
static const u8 pidff_set_condition[] = {
	PID_INDEX_PLACEHOLDER, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65
};

#define PID_MAGNITUDE	1
#define PID_OFFSET		2
#define PID_PHASE		3
#define PID_PERIOD		4
static const u8 pidff_set_periodic[] = {
	PID_INDEX_PLACEHOLDER, 0x70, 0x6f, 0x71, 0x72
};

static const u8 pidff_set_constant[] = { PID_INDEX_PLACEHOLDER, 0x70 };

#define PID_RAMP_START		1
#define PID_RAMP_END		2
static const u8 pidff_set_ramp[] = { PID_INDEX_PLACEHOLDER, 0x75, 0x76 };

#define PID_RAM_POOL_AVAILABLE	1
static const u8 pidff_block_load[] = { 0x22, 0xac };

#define PID_LOOP_COUNT		1
static const u8 pidff_effect_operation[] = { 0x22, 0x7c };

static const u8 pidff_block_free[] = { 0x22 };

#define PID_DEVICE_GAIN_FIELD	0
static const u8 pidff_device_gain[] = { 0x7e };

#define PID_RAM_POOL_SIZE		0
#define PID_SIMULTANEOUS_MAX	1
#define PID_DEVICE_MANAGED_POOL	2
#define PID_POOL_ALIGNMENT		3
static const u8 pidff_pool[] = { 0x80, 0x83, 0xa9, 0x84 };

#define PID_MOVE_SOURCE		0
#define PID_MOVE_DESTINATION	1
#define PID_MOVE_LENGTH			2
static const u8 pidff_pool_move[] = { 0x86, 0x87, 0x88 };

/* Special field key tables used to put special field keys into arrays */

#define PID_ENABLE_ACTUATORS	0
#define PID_RESET		1
static const u8 pidff_device_control[] = { 0x97, 0x9a };

#define PID_CONSTANT	0
#define PID_RAMP	1
#define PID_SQUARE	2
#define PID_SINE	3
#define PID_TRIANGLE	4
#define PID_SAW_UP	5
#define PID_SAW_DOWN	6
#define PID_SPRING	7
#define PID_DAMPER	8
#define PID_INERTIA	9
#define PID_FRICTION	10
static const u8 pidff_effect_types[] = {
	0x26, 0x27, 0x30, 0x31, 0x32, 0x33, 0x34,
	0x40, 0x41, 0x42, 0x43
};

#define PID_BLOCK_LOAD_SUCCESS	0
#define PID_BLOCK_LOAD_FULL	1
static const u8 pidff_block_load_status[] = { 0x8c, 0x8d };

#define PID_EFFECT_START	0
#define PID_EFFECT_STOP		1
static const u8 pidff_effect_operation_status[] = { 0x79, 0x7b };

/* Flags to indicate capabilities of the device */

#define PID_SUPPORTS_DEVICE_MANAGED		1
#define PID_SUPPORTS_POOL_MOVE			2

struct pidff_usage {
	struct hid_field *field;
	s32 *value;
};

struct pidff_memory_block {
	unsigned int block_index;	/* Effect block index */
	unsigned int block_offset;	/* Effect block offset */
	unsigned int size;		/* Block size (reserved memory) */
	u8 offset_num;

	struct list_head list;
};

struct pidff_info {
	int id;
	int effect_type_id;
	struct pidff_memory_block *offset[PID_AXES_MAX];
};

struct pidff_device {
	struct hid_device *hid;

	struct hid_report *reports[sizeof(pidff_reports)];
	int report_size[sizeof(pidff_reports)];

	struct pidff_usage set_effect[sizeof(pidff_set_effect)];
	struct pidff_usage set_effect_optional[sizeof(pidff_set_effect_optional)];
	struct pidff_usage block_offset[PID_AXES_MAX];
	struct pidff_usage set_envelope[sizeof(pidff_set_envelope)];
	struct pidff_usage set_condition[sizeof(pidff_set_condition)];
	struct pidff_usage set_periodic[sizeof(pidff_set_periodic)];
	struct pidff_usage set_constant[sizeof(pidff_set_constant)];
	struct pidff_usage set_ramp[sizeof(pidff_set_ramp)];

	struct pidff_usage device_gain[sizeof(pidff_device_gain)];
	struct pidff_usage block_load[sizeof(pidff_block_load)];
	struct pidff_usage pool[sizeof(pidff_pool)];
	struct pidff_usage pool_move[sizeof(pidff_pool_move)];
	struct pidff_usage effect_operation[sizeof(pidff_effect_operation)];
	struct pidff_usage block_free[sizeof(pidff_block_free)];

	/* Special field is a field that is not composed of
	 * usage<->value pairs that pidff_usage values are
	 */

	/* Special field in create_new_effect */
	struct hid_field *create_new_effect_type;

	/* Special fields in set_effect */
	struct hid_field *set_effect_type;
	struct hid_field *effect_direction;
	struct hid_field *axes_enable;

	/* Special field in device_control */
	struct hid_field *device_control;

	/* Special field in block_load */
	struct hid_field *block_load_status;

	/* Special field in effect_operation */
	struct hid_field *effect_operation_status;

	int control_id[sizeof(pidff_device_control)];
	int type_id[sizeof(pidff_effect_types)];
	int status_id[sizeof(pidff_block_load_status)];
	int operation_id[sizeof(pidff_effect_operation_status)];

	unsigned int pid_total_ram, pid_used_ram;
	int max_effects;

	unsigned long flags;

	struct pidff_info effect[PID_EFFECTS_MAX];
	struct pidff_info active;
	int active_effect_id;

	/* struct pidff_memory_block *memory; */
	struct list_head memory;
	int alignment;
};

/*
 * Calculate a report storage size in device memory, i.e. the report size
 * minus block offsets and effect id. Calculation is used as a fallback, if
 * device does not fill the report sizes in PID_POOL. The resulting size may
 * not be correct, but it's the best guess we can do...
 */
static int pidff_calculate_report_store_size(struct hid_report *report,
	struct pidff_usage *usage)
{
	if (report->field[0]->logical == (HID_UP_PID |
		pidff_reports[PID_SET_EFFECT]))
		return (report->size
			- usage[PID_EFFECT_BLOCK_INDEX].field->report_size) / 8;
	else
		return (report->size
			- usage[PID_PARAM_BLOCK_OFFSET].field->report_size) / 8;
}

/*
 * Get the report size when stored into device memory.
 */
static int pidff_report_store_size(struct pidff_device *pidff, int report)
{
	int size;

	size = pidff->report_size[report];
	if (size) {
		/* hid_dbg(pidff->hid, "Report %d size %d.\n", report, size); */
		return size;
	}

	/* Fallback, calculate size */
	hid_warn(pidff->hid, "Calculating size for report %d.\n", report);
	switch (report) {
	case PID_SET_EFFECT:
		/* Calculation removed since it seemed to cause issues
		 * pidff->report_size[report] = pidff_calculate_report_store_size(
		 *	pidff->reports[PID_SET_EFFECT], pidff->set_effect);
		 */
		/* If set report is not defined, set it to zero (not stored in pool) */
		pidff->report_size[report] = 0;
		break;
	case PID_SET_ENVELOPE:
		pidff->report_size[report] = pidff_calculate_report_store_size(
			pidff->reports[PID_SET_ENVELOPE], pidff->set_envelope);
		break;
	case PID_SET_CONDITION:
		pidff->report_size[report] = pidff_calculate_report_store_size(
			pidff->reports[PID_SET_CONDITION],
			pidff->set_condition);
		break;
	case PID_SET_PERIODIC:
		pidff->report_size[report] = pidff_calculate_report_store_size(
			pidff->reports[PID_SET_PERIODIC], pidff->set_periodic);
		break;
	case PID_SET_CONSTANT:
		pidff->report_size[report] = pidff_calculate_report_store_size(
			pidff->reports[PID_SET_CONSTANT], pidff->set_constant);
		break;
	case PID_SET_RAMP:
		pidff->report_size[report] = pidff_calculate_report_store_size(
			pidff->reports[PID_SET_RAMP], pidff->set_ramp);
		break;
	default:
		hid_dbg(pidff->hid, "Unknown report size queried\n");
		return 0;
	};
	hid_warn(pidff->hid, "Calculated to be %d bytes.\n", pidff->report_size[report]);
	return pidff->report_size[report];
}

/*
 * Find report store sizes in PID Pool report, if in driver managed mode.
 */
static void pidff_report_sizes(struct pidff_device *pidff)
{
	int i, j, k;
	struct hid_report *report = pidff->reports[PID_POOL];

	if (!report)
		return;

	for (i = 0; i < report->maxfield; i++) {
		if (report->field[i]->maxusage !=
			report->field[i]->report_count)
			continue;

		/* Parameter block size report = 0xA8 */
		if ((report->field[i]->logical & 0xff) != 0xa8)
			continue;

		for (j = 0; j < report->field[i]->maxusage; j++) {
			for (k = 0; k < sizeof(pidff_reports); k++) {
				if (pidff_reports[k] ==
					(report->field[i]->usage[j].hid
					& 0xff)) {
					pidff->report_size[k] =
						report->field[i]->value[j];
					break;
				}
			}
		}
		break;
	}
}

/*
 * Return a new free memory block offset. NULL on error.
 */
static struct pidff_memory_block *pidff_allocate_memory_block(
		struct pidff_device *pidff, int size)
{
	struct pidff_memory_block *block, *new_block;
	int offset, free_mem;

	if (!size)
		return NULL;

	/* Make sure alignment is as the device wants */
	size += size % pidff->alignment;

	if (pidff->pid_total_ram < (pidff->pid_used_ram + size))
		return NULL;

	if (list_empty(&pidff->memory)) {
		new_block = kzalloc(sizeof(*new_block),
			GFP_KERNEL);
		if (!new_block)
			return NULL;

		list_add(&new_block->list, &pidff->memory);

		offset = pidff_report_store_size(pidff, PID_SET_EFFECT);
		if (offset == 0) {
			/* If SET_EFFECT report size is not defined, assume they
			 * are not stored in pool and blocks can be stored at the
			 * beginning of pool. However address 0 does not seem to
			 * work so starting at first aligned offset.
			 */
			if(pidff->block_offset[0].field) {
				offset = pidff->block_offset[0].field->logical_minimum;
			} else {
				offset = pidff->alignment;	/* This is a hack... */
			}
		} else {
			/* If SET_EFFECT size was defined, assume they are stored
			 * at the beginning of pool so the blocks must start after
			 * the maximum amount of effects.
			 */
			offset += offset % pidff->alignment;
			offset *= pidff->max_effects;
		}

		new_block->block_index = pidff->active.id;
		new_block->block_offset = offset;
		new_block->size = size;

		pidff->pid_used_ram = offset + size;
#ifdef DEBUG_MEM_ALLOC
		hid_dbg(pidff->hid, "First block allocated at 0x%x, size %d, ram used %d\n",
			new_block->block_offset, new_block->size,
			pidff->pid_used_ram);
#endif
		return new_block;
	}

	/* Try to find first large enough memory slot */
	list_for_each_entry(block, &pidff->memory, list) {
		if (!list_is_last(&block->list, &pidff->memory)) {
			free_mem = list_next_entry(block, list)->block_offset -
				block->block_offset - block->size;

			/* Found large enough memory slot */
			if (free_mem >= size) {
				new_block = kzalloc(sizeof(*new_block),	GFP_KERNEL);
				if (!new_block)
					return NULL;

				offset = block->block_offset + block->size;
				new_block->block_index = pidff->active.id;
				new_block->block_offset = offset;
				new_block->size = size;

				list_add(&new_block->list, &block->list);

				pidff->pid_used_ram += size;

#ifdef DEBUG_MEM_ALLOC
				hid_dbg(pidff->hid, "Block allocated at 0x%x size %d, ram used%d\n",
					new_block->block_offset,
					new_block->size,
					pidff->pid_used_ram);
#endif
				return new_block;
			}

		/* Last block and large enough area at the end */
		} else if(size <= (pidff->pid_total_ram -
						block->block_offset - block->size)) {
			new_block = kzalloc(sizeof(*new_block),	GFP_KERNEL);
			if (!new_block)
				return NULL;

			offset = block->block_offset + block->size;
			new_block->block_index = pidff->active.id;
			new_block->block_offset = offset;
			new_block->size = size;

			list_add(&new_block->list, &block->list);

			pidff->pid_used_ram += size;

#ifdef DEBUG_MEM_ALLOC
			hid_dbg(pidff->hid, "Block allocated at 0x%x size %d, ram used %d\n",
				new_block->block_offset, size,
				pidff->pid_used_ram);
#endif
			return new_block;

		} else {
			/* TODO: Enough free memory but not in consecutive
			 * area so need to move old blocks around (defragment)
			 * to fit the new effect. For now, just fail.
			 */
			return NULL;
		}
	}
	return NULL;
}

/*
 * Free whole memory, i.e. delete the linked list
 */
static void pidff_empty_memory(struct pidff_device *pidff)
{
	struct pidff_memory_block *block, *temp;

	list_for_each_entry_safe(block, temp, &pidff->memory, list) {
		list_del(&block->list);
		kfree(block);
	}

	pidff->pid_used_ram = 0;
}

/*
 * Free an allocated memory block
 */
static void pidff_free_memory_block(struct pidff_device *pidff,
		struct pidff_memory_block *block)
{
	list_del(&block->list);
	pidff->pid_used_ram -= block->size;
#ifdef DEBUG_MEM_ALLOC
	hid_dbg(pidff->hid, "Block freed from 0x%x, ram used %d\n",
		block->block_offset, pidff->pid_used_ram);
#endif
	kfree(block);
}

/*
 * Get existing block or allocate a new block for effect info.
 * n is which block offset/axis is used.
 * 1 is magnitude, period, ramp or X axis
 * 2 is envelope or Y axis
 * Returns the offset or -1 on error.
 */
static int pidff_get_or_allocate_block(struct pidff_device *pidff,
		int effect_id, int size, int n)
{
	int i, offset;
	struct pidff_memory_block *block;

	/* Offsets start from 1..., scale to 0... */
	n--;
	if (n < 0 || n >= PID_AXES_MAX)
		return -1;

	/* Make sure the size alignment is correct */
	size += size % pidff->alignment;

	for (i = 0; i < pidff->max_effects; i++) {
		if (pidff->effect[i].id == effect_id) {
			if (!pidff->effect[i].offset[n]) {
				/* Memory not yet allocated */
				block = pidff_allocate_memory_block(pidff, size);
				if (!block)
					return -1;

				offset = block->block_offset;
				block->offset_num = n;
				pidff->effect[i].offset[n] = block;

#ifdef DEBUG_MEM_ALLOC
				hid_dbg(pidff->hid, "New block allocated");
#endif
			} else if (pidff->effect[i].offset[n]->size == size) {
				/* Block can be re-used */
				offset = pidff->effect[i].offset[n]->block_offset;
#ifdef DEBUG_MEM_ALLOC
				hid_dbg(pidff->hid, "Block re-used");
#endif
			} else {
				/* Block was wrong size */
#ifdef DEBUG_MEM_ALLOC
				hid_dbg(pidff->hid, "Wrong size %d!=%d block re-allocated", pidff->effect[i].offset[n]->size, size);
#endif
				pidff_free_memory_block(pidff, pidff->effect[i].offset[n]);
				block = pidff_allocate_memory_block(pidff, size);
				if (!block)
					return -1;

				offset = block->block_offset;
				block->offset_num = n;
				pidff->effect[i].offset[n] = block;
			}
#ifdef DEBUG_MEM_ALLOC
			hid_dbg(pidff->hid, "Block for %d (%d) at 0x%x\n",
				effect_id, n+1, offset);
#endif
			return offset;
		}
	}
	return -1;
}

/*
 * Scale an unsigned value with range 0..max for the given field
 */
static int pidff_rescale(int i, int max, struct hid_field *field)
{
	return i * (field->logical_maximum - field->logical_minimum) / max +
	    field->logical_minimum;
}

/*
 * Scale a signed value in range -0x8000..0x7fff for the given field
 */
static int pidff_rescale_signed(int i, struct hid_field *field)
{
	return i == 0 ? 0 : i >
	    0 ? i * field->logical_maximum / 0x7fff : i *
	    field->logical_minimum / -0x8000;
}

static void pidff_set(struct pidff_usage *usage, u16 value)
{
	if (!usage)
		return;
	usage->value[0] = pidff_rescale(value, 0xffff, usage->field);
#ifdef DEBUG_SCALING
	pr_debug("calculated from %d to %d\n", value, usage->value[0]);
#endif
}

static void pidff_set_signed(struct pidff_usage *usage, s16 value)
{
	if (usage->field->logical_minimum < 0)
		usage->value[0] = pidff_rescale_signed(value, usage->field);
	else {
		if (value < 0)
			usage->value[0] =
			    pidff_rescale(-value, 0x8000, usage->field);
		else
			usage->value[0] =
			    pidff_rescale(value, 0x7fff, usage->field);
	}
#ifdef DEBUG_SCALING
	pr_debug("calculated from %d to %d\n", value, usage->value[0]);
#endif
}

/*
 * Send envelope report to the device
 */
static int pidff_set_envelope_report(struct pidff_device *pidff,
				      struct ff_envelope *envelope)
{
	int offset;

	if (IS_DEVICE_MANAGED(pidff)) {
		pidff->set_envelope[PID_EFFECT_BLOCK_INDEX].value[0] =
		    pidff->active.id;
	} else {
		offset = pidff_get_or_allocate_block(pidff, pidff->active.id,
			pidff_report_store_size(pidff,
			PID_SET_ENVELOPE), 2);
		if (offset < 0)
			return -ENOSPC;

		pidff->set_envelope[PID_PARAM_BLOCK_OFFSET].value[0] = offset;
	}

	pidff->set_envelope[PID_ATTACK_LEVEL].value[0] =
	    pidff_rescale(envelope->attack_level >
		0x7fff ? 0x7fff : envelope->attack_level, 0x7fff,
		pidff->set_envelope[PID_ATTACK_LEVEL].field);
	pidff->set_envelope[PID_FADE_LEVEL].value[0] =
	    pidff_rescale(envelope->fade_level >
		0x7fff ? 0x7fff : envelope->fade_level, 0x7fff,
		pidff->set_envelope[PID_FADE_LEVEL].field);

	pidff->set_envelope[PID_ATTACK_TIME].value[0] = envelope->attack_length;
	pidff->set_envelope[PID_FADE_TIME].value[0] = envelope->fade_length;

#ifdef DEBUG_SCALING
	hid_dbg(pidff->hid, "attack %u => %d\n",
		envelope->attack_level,
		pidff->set_envelope[PID_ATTACK_LEVEL].value[0]);
#endif

	hid_hw_request(pidff->hid, pidff->reports[PID_SET_ENVELOPE],
		HID_REQ_SET_REPORT);
	return 0;
}

/*
 * Test if the new envelope differs from old one
 */
static int pidff_needs_set_envelope(struct ff_envelope *envelope,
				    struct ff_envelope *old)
{
	return envelope->attack_level != old->attack_level ||
	       envelope->fade_level != old->fade_level ||
	       envelope->attack_length != old->attack_length ||
	       envelope->fade_length != old->fade_length;
}

/*
 * Send constant force report to the device
 */
static int pidff_set_constant_force_report(struct pidff_device *pidff,
					    struct ff_effect *effect)
{
	int offset;

	if (IS_DEVICE_MANAGED(pidff)) {
		pidff->set_constant[PID_EFFECT_BLOCK_INDEX].value[0] =
			pidff->active.id;
	} else {
		offset = pidff_get_or_allocate_block(pidff, pidff->active.id,
			pidff_report_store_size(pidff,
			PID_SET_CONSTANT), 1);
		if (offset < 0)
			return -ENOSPC;

		pidff->set_constant[PID_PARAM_BLOCK_OFFSET].value[0] = offset;
	}

	pidff_set_signed(&pidff->set_constant[PID_MAGNITUDE],
		effect->u.constant.level);

	hid_hw_request(pidff->hid, pidff->reports[PID_SET_CONSTANT],
		HID_REQ_SET_REPORT);
	return 0;
}

/*
 * Test if the constant parameters have changed between effects
 */
static int pidff_needs_set_constant(struct ff_effect *effect,
				    struct ff_effect *old)
{
	return effect->u.constant.level != old->u.constant.level;
}

/*
 * Send set effect report to the device
 */
static void pidff_set_effect_report(struct pidff_device *pidff,
				    struct ff_effect *effect)
{
	int i;

	pidff->set_effect[PID_EFFECT_BLOCK_INDEX].value[0] =
		pidff->active.id;

	if (IS_DEVICE_MANAGED(pidff)) {
		pidff->set_effect_type->value[0] =
			pidff->create_new_effect_type->value[0];
	} else {
		pidff->set_effect_type->value[0] = pidff->active.effect_type_id;
		if (pidff->active.offset[0]) {
			pidff->block_offset[0].value[0] = pidff->active.
				offset[0]->block_offset;
			/* hid_dbg(pidff->hid, "Has offset 1 at 0x%x\n", pidff->active.offset[0]->block_offset); */
		}
		else
			pidff->block_offset[0].value[0] = 0;

		if (pidff->active.offset[1]) {
			pidff->block_offset[1].value[0] = pidff->active.
				offset[1]->block_offset;
			/* hid_dbg(pidff->hid, "Has offset 2 at 0x%x\n", pidff->active.offset[1]->block_offset); */
		}
		else
			pidff->block_offset[1].value[0] = 0;
	}

	if (effect->replay.length == 0) {
		pidff->set_effect[PID_DURATION].value[0] =
			(1U << pidff->set_effect[PID_DURATION].field->report_size) - 1;
		hid_dbg(pidff->hid, "Set inf. length (%d) to 0x%x\n",
			pidff->set_effect[PID_DURATION].field->report_size,
			(1U << pidff->set_effect[PID_DURATION].field->report_size) - 1);
	} else {
		pidff->set_effect[PID_DURATION].value[0] =
			effect->replay.length;
	}

	pidff->set_effect[PID_TRIGGER_BUTTON].value[0] = effect->trigger.button;
	pidff->set_effect[PID_TRIGGER_REPEAT_INT].value[0] =
		effect->trigger.interval;
	if (pidff->set_effect_optional[PID_GAIN].value)
		pidff->set_effect_optional[PID_GAIN].value[0] =
			pidff->set_effect_optional[PID_GAIN].field->logical_maximum;


	if ((effect->type == FF_SPRING || effect->type == FF_DAMPER ||
		effect->type == FF_FRICTION || effect->type == FF_INERTIA) &&
		pidff->axes_enable) {
		pidff->set_effect[PID_DIRECTION_ENABLE].value[0] = 0;

		for (i = 0; i < pidff->axes_enable->report_count; i++) {
			pidff->axes_enable->value[i] = 1;
		}
	} else {
		pidff->set_effect[PID_DIRECTION_ENABLE].value[0] = 1;
		pidff->effect_direction->value[0] =
			pidff_rescale(effect->direction, 0xffff,
				pidff->effect_direction);
	}

	pidff->set_effect[PID_START_DELAY].value[0] = effect->replay.delay;

	hid_hw_request(pidff->hid, pidff->reports[PID_SET_EFFECT],
		HID_REQ_SET_REPORT);
}

/*
 * Test if the values used in set_effect have changed
 */
static int pidff_needs_set_effect(struct ff_effect *effect,
				  struct ff_effect *old)
{
	return effect->replay.length != old->replay.length ||
	       effect->trigger.interval != old->trigger.interval ||
	       effect->trigger.button != old->trigger.button ||
	       effect->direction != old->direction ||
	       effect->replay.delay != old->replay.delay;
}

/*
 * Send periodic effect report to the device
 */
static int pidff_set_periodic_report(struct pidff_device *pidff,
				      struct ff_effect *effect)
{
	int offset;

	if (IS_DEVICE_MANAGED(pidff)) {
		pidff->set_periodic[PID_EFFECT_BLOCK_INDEX].value[0] =
			pidff->active.id;
	} else {
		offset = pidff_get_or_allocate_block(pidff, pidff->active.id,
			pidff_report_store_size(pidff,
			PID_SET_PERIODIC), 1);
		if (offset < 0)
			return -ENOSPC;

		pidff->set_periodic[PID_PARAM_BLOCK_OFFSET].value[0] = offset;
	}

	pidff_set_signed(&pidff->set_periodic[PID_MAGNITUDE],
		effect->u.periodic.magnitude);
	pidff_set_signed(&pidff->set_periodic[PID_OFFSET],
		effect->u.periodic.offset);
	pidff_set(&pidff->set_periodic[PID_PHASE], effect->u.periodic.phase);
	pidff->set_periodic[PID_PERIOD].value[0] = effect->u.periodic.period;

	hid_hw_request(pidff->hid, pidff->reports[PID_SET_PERIODIC],
		HID_REQ_SET_REPORT);
	return 0;
}

/*
 * Test if periodic effect parameters have changed
 */
static int pidff_needs_set_periodic(struct ff_effect *effect,
				    struct ff_effect *old)
{
	return effect->u.periodic.magnitude != old->u.periodic.magnitude ||
	       effect->u.periodic.offset != old->u.periodic.offset ||
	       effect->u.periodic.phase != old->u.periodic.phase ||
	       effect->u.periodic.period != old->u.periodic.period;
}

/*
 * Send condition effect reports to the device
 */
static int pidff_set_condition_report(struct pidff_device *pidff,
				       struct ff_effect *effect)
{
	int i, offset;

	if (IS_DEVICE_MANAGED(pidff))
		pidff->set_condition[PID_EFFECT_BLOCK_INDEX].value[0] =
			pidff->active.id;

	for (i = 0; i < 2; i++) {
		if (IS_DEVICE_MANAGED(pidff)) {
			pidff->set_condition[PID_PARAM_BLOCK_OFFSET].value[0] =
				i;
		} else {
			offset = pidff_get_or_allocate_block(pidff,
				pidff->active.id, pidff_report_store_size(pidff,
				PID_SET_CONDITION), i+1);
			if (offset < 0)
				return -ENOSPC;

			pidff->set_condition[PID_PARAM_BLOCK_OFFSET].value[0] =
				offset;
		}

		pidff_set_signed(&pidff->set_condition[PID_CP_OFFSET],
			effect->u.condition[i].center);
		pidff_set_signed(&pidff->set_condition[PID_POS_COEFFICIENT],
			effect->u.condition[i].right_coeff);
		pidff_set_signed(&pidff->set_condition[PID_NEG_COEFFICIENT],
			effect->u.condition[i].left_coeff);
		pidff_set(&pidff->set_condition[PID_POS_SATURATION],
			effect->u.condition[i].right_saturation);
		pidff_set(&pidff->set_condition[PID_NEG_SATURATION],
			effect->u.condition[i].left_saturation);
		pidff_set(&pidff->set_condition[PID_DEAD_BAND],
			effect->u.condition[i].deadband);

		hid_hw_request(pidff->hid, pidff->reports[PID_SET_CONDITION],
			HID_REQ_SET_REPORT);
		hid_hw_wait(pidff->hid);
	}
	return 0;
}

/*
 * Test if condition effect parameters have changed
 */
static int pidff_needs_set_condition(struct ff_effect *effect,
				     struct ff_effect *old)
{
	int i;
	int ret = 0;

	for (i = 0; i < 2; i++) {
		struct ff_condition_effect *cond = &effect->u.condition[i];
		struct ff_condition_effect *old_cond = &old->u.condition[i];

		ret |= cond->center != old_cond->center ||
	       cond->right_coeff != old_cond->right_coeff ||
	       cond->left_coeff != old_cond->left_coeff ||
	       cond->right_saturation != old_cond->right_saturation ||
	       cond->left_saturation != old_cond->left_saturation ||
	       cond->deadband != old_cond->deadband;
	}

	return ret;
}

/*
 * Send ramp force report to the device
 */
static int pidff_set_ramp_force_report(struct pidff_device *pidff,
					struct ff_effect *effect)
{
	int offset;

	if (IS_DEVICE_MANAGED(pidff)) {
		pidff->set_ramp[PID_EFFECT_BLOCK_INDEX].value[0] =
			pidff->active.id;
	} else {
		offset = pidff_get_or_allocate_block(pidff, pidff->active.id,
			pidff_report_store_size(pidff, PID_SET_RAMP),
			1);
		if (offset < 0)
			return -ENOSPC;

		pidff->set_ramp[PID_PARAM_BLOCK_OFFSET].value[0] = offset;
	}

	pidff_set_signed(&pidff->set_ramp[PID_RAMP_START],
		 effect->u.ramp.start_level);
	pidff_set_signed(&pidff->set_ramp[PID_RAMP_END],
		 effect->u.ramp.end_level);
	hid_hw_request(pidff->hid, pidff->reports[PID_SET_RAMP],
		HID_REQ_SET_REPORT);
	return 0;
}

/*
 * Test if ramp force parameters have changed
 */
static int pidff_needs_set_ramp(struct ff_effect *effect, struct ff_effect *old)
{
	return effect->u.ramp.start_level != old->u.ramp.start_level ||
	       effect->u.ramp.end_level != old->u.ramp.end_level;
}

/*
 * Send a request for effect upload to the device
 *
 * Returns 0 if device reported success, -ENOSPC if the device reported memory
 * is full. Upon unknown response the function will retry for 60 times, if
 * still unsuccessful -EIO is returned.
 */
static int pidff_request_effect_upload(struct pidff_device *pidff, int efnum)
{
	int j;

	if (IS_DEVICE_MANAGED(pidff)) {
		pidff->create_new_effect_type->value[0] = efnum;
		hid_hw_request(pidff->hid,
			pidff->reports[PID_CREATE_NEW_EFFECT],
			HID_REQ_SET_REPORT);
		hid_dbg(pidff->hid, "create_new_effect sent, type: %d\n",
			efnum);

		pidff->block_load[PID_EFFECT_BLOCK_INDEX].value[0] = 0;
		pidff->block_load_status->value[0] = 0;
		hid_hw_wait(pidff->hid);

		for (j = 0; j < 60; j++) {
			hid_dbg(pidff->hid, "pid_block_load requested\n");
			hid_hw_request(pidff->hid,
				pidff->reports[PID_BLOCK_LOAD],
				HID_REQ_GET_REPORT);
			hid_hw_wait(pidff->hid);
			if (pidff->block_load_status->value[0] ==
				pidff->status_id[PID_BLOCK_LOAD_SUCCESS]) {
				hid_dbg(pidff->hid, "device reported free memory: %d bytes\n",
					pidff->block_load[PID_RAM_POOL_AVAILABLE].value ?
					pidff->block_load[PID_RAM_POOL_AVAILABLE].value[0] : -1);

				pidff->active.id = pidff->
					block_load[PID_EFFECT_BLOCK_INDEX].
					value[0];
				return 0;
			}
			if (pidff->block_load_status->value[0] ==
				pidff->status_id[PID_BLOCK_LOAD_FULL]) {
				hid_dbg(pidff->hid, "not enough memory free: %d bytes\n",
					pidff->block_load[PID_RAM_POOL_AVAILABLE].value ?
					pidff->block_load[PID_RAM_POOL_AVAILABLE].value[0] : -1);
				return -ENOSPC;
			}
		}
		hid_err(pidff->hid, "pid_block_load failed 60 times\n");

	} else {
		/* Driver managed mode, allocate a new id if any is available */
		for (j = 0; j < pidff->max_effects; j++) {
			if (pidff->effect[j].id > -1)
				continue;

			pidff->effect[pidff->active_effect_id].id =
				pidff->active.id = j;
			pidff->effect[pidff->active_effect_id].offset[0] = NULL;
			pidff->effect[pidff->active_effect_id].offset[1] = NULL;
			pidff->effect[pidff->active_effect_id].effect_type_id =
				pidff->active.effect_type_id = efnum;

			hid_dbg(pidff->hid, "upload id %d\n", pidff->active.id);
			return 0;
		}
		return -ENOSPC;
	}
	return -EIO;
}

/*
 * Play the effect with PID id n times
 */
static void pidff_playback_pid(struct pidff_device *pidff, int pid_id, int n)
{
	pidff->effect_operation[PID_EFFECT_BLOCK_INDEX].value[0] = pid_id;

	if (n == 0) {
		pidff->effect_operation_status->value[0] =
			pidff->operation_id[PID_EFFECT_STOP];
	} else {
		pidff->effect_operation_status->value[0] =
			pidff->operation_id[PID_EFFECT_START];
		pidff->effect_operation[PID_LOOP_COUNT].value[0] = n;
	}

	hid_hw_request(pidff->hid, pidff->reports[PID_EFFECT_OPERATION],
		HID_REQ_SET_REPORT);
}

/**
 * Play the effect with effect id @effect_id for @value times
 */
static int pidff_playback(struct input_dev *dev, int effect_id, int value)
{
	struct pidff_device *pidff = dev->ff->private;

	pidff_playback_pid(pidff, pidff->effect[effect_id].id, value);

	return 0;
}

/*
 * Erase effect with PID id
 */
static void pidff_erase_pid(struct pidff_device *pidff, int pid_id)
{
	struct list_head *pos, *temp;
	struct pidff_memory_block *block;

	if (IS_DEVICE_MANAGED(pidff)) {
		pidff->block_free[PID_EFFECT_BLOCK_INDEX].value[0] = pid_id;
		hid_hw_request(pidff->hid, pidff->reports[PID_BLOCK_FREE],
			HID_REQ_SET_REPORT);

	} else {
		list_for_each_safe(pos, temp, &pidff->memory) {
			block = list_entry(pos, struct pidff_memory_block, list);

			if (block->block_index == pid_id) {
#ifdef DEBUG_MEM_ALLOC
				hid_dbg(pidff->hid, "Block erased at 0x%x\n", block->block_offset);
#endif
				pidff_free_memory_block(pidff, block);
			}
		}
	}
}

/*
 * Stop and erase effect with effect_id
 */
static int pidff_erase_effect(struct input_dev *dev, int effect_id)
{
	struct pidff_device *pidff = dev->ff->private;
	int pid_id = pidff->effect[effect_id].id;

	hid_dbg(pidff->hid, "starting to erase %d/%d\n",
		effect_id, pidff->effect[effect_id].id);
	/* Wait for the queue to clear. We do not want a full fifo to
	   prevent the effect removal. */
	hid_hw_wait(pidff->hid);
	pidff_playback_pid(pidff, pid_id, 0);
	pidff_erase_pid(pidff, pid_id);

	pidff->effect[effect_id].id = -1;
	pidff->effect[effect_id].offset[0] = NULL;
	pidff->effect[effect_id].offset[1] = NULL;

	return 0;
}

/*
 * Effect upload handler
 */
static int pidff_upload_effect(struct input_dev *dev, struct ff_effect *effect,
			       struct ff_effect *old)
{
	struct pidff_device *pidff = dev->ff->private;
	int type_id = 0;
	int error = 0;
	int needs_set_effect = 0;
	int (*needs_set_report)(struct ff_effect *, struct ff_effect *) = NULL;
	int (*set_report_func)(struct pidff_device *, struct ff_effect *) =
		NULL;
	struct ff_envelope *envelope = NULL, *old_envelope = NULL;

	pidff->active_effect_id = effect->id;
	if (old) {
		pidff->active.id = pidff->effect[effect->id].id;
		pidff->active.offset[0] = pidff->effect[effect->id].offset[0];
		pidff->active.offset[1] = pidff->effect[effect->id].offset[1];

		if (pidff_needs_set_effect(effect, old))
			needs_set_effect = 1;
	} else {
		pidff->active.offset[0] = NULL;
		pidff->active.offset[1] = NULL;
		needs_set_effect = 1;
	}

	if(IS_DEVICE_MANAGED(pidff)) {
		pidff->block_load[PID_EFFECT_BLOCK_INDEX].value[0] = 0;
		if(old) {
			pidff->block_load[PID_EFFECT_BLOCK_INDEX].value[0] =
				pidff->active.id;
		}
	}

	switch (effect->type) {
	case FF_CONSTANT:
		needs_set_report = pidff_needs_set_constant;
		set_report_func = pidff_set_constant_force_report;
		envelope = &effect->u.constant.envelope;
		old_envelope = &old->u.constant.envelope;
		type_id = PID_CONSTANT;
		break;

	case FF_PERIODIC:
		if (!old) {
			switch (effect->u.periodic.waveform) {
			case FF_SQUARE:
				type_id = PID_SQUARE;
				break;
			case FF_TRIANGLE:
				type_id = PID_TRIANGLE;
				break;
			case FF_SINE:
				type_id = PID_SINE;
				break;
			case FF_SAW_UP:
				type_id = PID_SAW_UP;
				break;
			case FF_SAW_DOWN:
				type_id = PID_SAW_DOWN;
				break;
			default:
				hid_err(pidff->hid, "invalid waveform\n");
				return -EINVAL;
			}
		}

		needs_set_report = pidff_needs_set_periodic;
		set_report_func = pidff_set_periodic_report;
		envelope = &effect->u.periodic.envelope;
		old_envelope = &old->u.periodic.envelope;
		break;

	case FF_RAMP:
		needs_set_report = pidff_needs_set_ramp;
		set_report_func = pidff_set_ramp_force_report;
		envelope = &effect->u.ramp.envelope;
		old_envelope = &old->u.ramp.envelope;
		type_id = PID_RAMP;
		break;

	case FF_SPRING:
		needs_set_report = pidff_needs_set_condition;
		set_report_func = pidff_set_condition_report;
		type_id = PID_SPRING;
		break;

	case FF_FRICTION:
		needs_set_report = pidff_needs_set_condition;
		set_report_func = pidff_set_condition_report;
		type_id = PID_FRICTION;
		break;

	case FF_DAMPER:
		needs_set_report = pidff_needs_set_condition;
		set_report_func = pidff_set_condition_report;
		type_id = PID_DAMPER;
		break;

	case FF_INERTIA:
		needs_set_report = pidff_needs_set_condition;
		set_report_func = pidff_set_condition_report;
		type_id = PID_INERTIA;
		break;

	default:
		hid_err(pidff->hid, "invalid type\n");
		return -EINVAL;
	}

	if (!old) {
		error = pidff_request_effect_upload(pidff,
			pidff->type_id[type_id]);
		if (error)
			return error;
	}

	if (needs_set_effect && IS_DEVICE_MANAGED(pidff))
		pidff_set_effect_report(pidff, effect);

	if ((!old || (needs_set_report && (*needs_set_report)(effect, old))) &&
		set_report_func) {
		error = (*set_report_func)(pidff, effect);
		if (error)
			goto fail;
	}

	if (envelope &&	(!old ||
		pidff_needs_set_envelope(envelope, old_envelope))) {
		error = pidff_set_envelope_report(pidff, envelope);
		if (error)
			goto fail;
	}

	if (!IS_DEVICE_MANAGED(pidff)) {
		if (!old || pidff->active.offset[0] !=
			pidff->effect[effect->id].offset[0] ||
			pidff->active.offset[1] !=
			pidff->effect[effect->id].offset[1] ||
			needs_set_effect) {

			pidff->active.offset[0] =
				pidff->effect[effect->id].offset[0];
			pidff->active.offset[1] =
				pidff->effect[effect->id].offset[1];
			pidff_set_effect_report(pidff, effect);
		}
	}

	/* hid_dbg(pidff->hid, "uploaded\n"); */
	pidff->active.id = -1;
	pidff->active.offset[0]	= NULL;
	pidff->active.offset[1] = NULL;
	return 0;

fail:
	hid_dbg(pidff->hid, "upload failed\n");
	pidff_erase_pid(pidff, pidff->active.id);
	pidff->active.id = -1;
	pidff->active.offset[0]	= NULL;
	pidff->active.offset[1] = NULL;
	return error;
}

/*
 * set_gain() handler
 */
static void pidff_set_gain(struct input_dev *dev, u16 gain)
{
	struct pidff_device *pidff = dev->ff->private;

	pidff_set(&pidff->device_gain[PID_DEVICE_GAIN_FIELD], gain);
	hid_hw_request(pidff->hid, pidff->reports[PID_DEVICE_GAIN],
			HID_REQ_SET_REPORT);
}

static void pidff_autocenter(struct pidff_device *pidff, u16 magnitude)
{
	struct hid_field *field;

	if (IS_DEVICE_MANAGED(pidff)) {
		field = pidff->block_load[PID_EFFECT_BLOCK_INDEX].field;

		if (!magnitude) {
			pidff_playback_pid(pidff, field->logical_minimum, 0);
			return;
		}

		pidff_playback_pid(pidff, field->logical_minimum, 1);

		pidff->set_effect[PID_EFFECT_BLOCK_INDEX].value[0] =
			pidff->block_load[PID_EFFECT_BLOCK_INDEX].field->
			logical_minimum;

		pidff->set_effect_type->value[0] = pidff->type_id[PID_SPRING];

		pidff->set_effect[PID_DURATION].value[0] = 0;
		pidff->set_effect[PID_TRIGGER_BUTTON].value[0] = 0;
		pidff->set_effect[PID_TRIGGER_REPEAT_INT].value[0] = 0;
		pidff_set(&pidff->set_effect_optional[PID_GAIN], magnitude);
		pidff->set_effect[PID_DIRECTION_ENABLE].value[0] = 1;
		pidff->set_effect[PID_START_DELAY].value[0] = 0;

		hid_hw_request(pidff->hid, pidff->reports[PID_SET_EFFECT],
			HID_REQ_SET_REPORT);
	}
}

/*
 * pidff_set_autocenter() handler
 */
static void pidff_set_autocenter(struct input_dev *dev, u16 magnitude)
{
	struct pidff_device *pidff = dev->ff->private;

	pidff_autocenter(pidff, magnitude);
}

/*
 * Find fields from a report and fill a pidff_usage
 */
static int pidff_find_fields(struct pidff_usage *usage, const u8 *table,
		struct hid_report *report, int count, int strict,
		struct pidff_device *dev)
{
	int i, j, k, found, key, offset;

	for (k = 0; k < count; k++) {
		found = 0;

		key = table[k];
		if (key == PID_INDEX_PLACEHOLDER) {
			if (IS_DEVICE_MANAGED(dev))
				key = PID_EFFECT_BLOCK_INDEX_CODE;
			else
				key = PID_PARAM_BLOCK_OFFSET_CODE;
		}

		for (i = 0; i < report->maxfield; i++) {
			if (report->field[i]->maxusage !=
			    report->field[i]->report_count) {
				pr_debug("maxusage and report_count do not match, skipping\n");
				continue;
			}
			for (j = 0; j < report->field[i]->maxusage; j++) {
				if (report->field[i]->usage[j].hid ==
				    (HID_UP_PID | key)) {
					pr_debug("found %d at %d->%d (0x%02d)\n",
						 k, i, j, key);
					usage[k].field = report->field[i];
					usage[k].value =
						&report->field[i]->value[j];
					found = 1;
					break;
				}

				/* Find block offsets for set effect report */
				if ((report->field[i]->logical & 0xff) ==
						0x58) {
					offset = (report->field[i]->usage[j].hid
						& 0xff) - 1;
					if (offset >= 0 && offset <
						PID_AXES_MAX &&
						!dev->block_offset[offset].field) {

						dev->block_offset[offset].field
							= report->field[i];
						dev->block_offset[offset].value
							= &report->field[i]->
							value[j];
					}
				}
			}
			if (found)
				break;
			}
		if (!found && strict) {
			pr_debug("failed to locate %d (0x%02x)\n", k, table[k]);
			return -1;
		}
	}
	return 0;
}

/*
 * Return index into pidff_reports for the given usage
 */
static int pidff_check_usage(int usage)
{
	int i;

	for (i = 0; i < sizeof(pidff_reports); i++)
		if (usage == (HID_UP_PID | pidff_reports[i]))
			return i;

	return -1;
}

/*
 * Find the reports and fill pidff->reports[]
 * report_type specifies either OUTPUT or FEATURE reports
 */
static void pidff_find_reports(struct hid_device *hid, int report_type,
			       struct pidff_device *pidff)
{
	struct hid_report *report;
	int i, ret;

	list_for_each_entry(report,
			    &hid->report_enum[report_type].report_list, list) {
		if (report->maxfield < 1)
			continue;
		ret = pidff_check_usage(report->field[0]->logical);
		if (ret != -1) {
			hid_dbg(hid, "found usage 0x%02x from field->logical, size %d\n",
				pidff_reports[ret], report->size);
			pidff->reports[ret] = report;
			continue;
		}

		/*
		 * Sometimes logical collections are stacked to indicate
		 * different usages for the report and the field, in which
		 * case we want the usage of the parent. However, Linux HID
		 * implementation hides this fact, so we have to dig it up
		 * ourselves
		 */
		i = report->field[0]->usage[0].collection_index;
		if (i <= 0 ||
		    hid->collection[i - 1].type != HID_COLLECTION_LOGICAL)
			continue;
		ret = pidff_check_usage(hid->collection[i - 1].usage);
		if (ret != -1 && !pidff->reports[ret]) {
			hid_dbg(hid,
				"found usage 0x%02x from collection array, size %d\n",
				pidff_reports[ret], report->size);
			pidff->reports[ret] = report;
		}
	}
}

/*
 * Test if the required reports have been found
 */
static int pidff_reports_ok(struct pidff_device *pidff)
{
	int i;

	/* Check for reports requried for all devices */
	for (i = 0; i <= PID_REQUIRED_REPORTS; i++) {
		if (!pidff->reports[i]) {
			hid_dbg(pidff->hid, "%d missing\n", i);
			return 0;
		}
	}

	/* Check for reports required for device managed mode */
	for (; i <= PID_REQUIRED_DEVICE_MANAGED; i++) {
		if (!pidff->reports[i]) {
			hid_dbg(pidff->hid, "%d missing for device managed\n",
				i);
			clear_bit(PID_SUPPORTS_DEVICE_MANAGED, &pidff->flags);
			break;
		}
	}

	/* Check for reports required for pool move (garbage collect) */
	for (i = PID_REQUIRED_DEVICE_MANAGED; i <= PID_REQUIRED_POOL_MOVE;
						i++) {
		if (!pidff->reports[i]) {
			hid_dbg(pidff->hid, "%d missing for pool moving\n", i);
			clear_bit(PID_SUPPORTS_POOL_MOVE, &pidff->flags);
			break;
		}
	}

	return 1;
}

/*
 * Find a field with a specific usage within a report
 */
static struct hid_field *pidff_find_special_field(struct hid_report *report,
						  int usage, int enforce_min)
{
	int i;

	for (i = 0; i < report->maxfield; i++) {
		if (report->field[i]->logical == (HID_UP_PID | usage) &&
		    report->field[i]->report_count > 0) {
			if (!enforce_min ||
			    report->field[i]->logical_minimum == 1) {
				return report->field[i];
			}

			pr_err("logical_minimum is not 1 as it should be\n");
			return NULL;
		}

		if (((report->field[i]->usage[0].hid & 0xff) == usage) &&
		    report->field[i]->report_count > 0) {
			if (!enforce_min ||
			    report->field[i]->logical_minimum == 1) {
				return report->field[i];
			}

			pr_err("logical_minimum is not 1 as it should be\n");
			return NULL;
		}
	}
	return NULL;
}

/*
 * Fill a pidff->*_id struct table
 */
static int pidff_find_special_keys(int *keys, struct hid_field *fld,
				   const u8 *usagetable, int count)
{

	int i, j;
	int found = 0;

	for (i = 0; i < count; i++) {
		for (j = 0; j < fld->maxusage; j++) {
			if (fld->usage[j].hid == (HID_UP_PID | usagetable[i])) {
				keys[i] = j + 1;
				found++;
				break;
			}
		}
	}
	return found;
}

#define PIDFF_FIND_SPECIAL_KEYS(keys, field, name) \
	pidff_find_special_keys(pidff->keys, pidff->field, pidff_ ## name, \
		sizeof(pidff_ ## name))
/*
 * Find and check the special fields
 */
static int pidff_find_special_fields(struct pidff_device *pidff)
{
	hid_dbg(pidff->hid, "finding special fields\n");

	pidff->set_effect_type =
		pidff_find_special_field(pidff->reports[PID_SET_EFFECT],
					 0x25, 1);
	pidff->effect_direction =
		pidff_find_special_field(pidff->reports[PID_SET_EFFECT],
					 0x57, 0);
	pidff->axes_enable =
		pidff_find_special_field(pidff->reports[PID_SET_EFFECT],
					 0x55, 0);
	pidff->device_control =
		pidff_find_special_field(pidff->reports[PID_DEVICE_CONTROL],
					 0x96, 1);
	pidff->effect_operation_status =
		pidff_find_special_field(pidff->reports[PID_EFFECT_OPERATION],
					 0x78, 1);

	if (IS_DEVICE_MANAGED(pidff)) {
		pidff->create_new_effect_type =
			pidff_find_special_field(
			pidff->reports[PID_CREATE_NEW_EFFECT], 0x25, 1);
		pidff->block_load_status =
			pidff_find_special_field(pidff->reports[PID_BLOCK_LOAD],
						0x8b, 1);
	}

	hid_dbg(pidff->hid, "search done\n");

	if ((!pidff->create_new_effect_type && IS_DEVICE_MANAGED(pidff))
			|| !pidff->set_effect_type) {
		hid_err(pidff->hid, "effect lists not found\n");
		return -1;
	}

	if (!pidff->effect_direction) {
		hid_err(pidff->hid, "direction field not found\n");
		return -1;
	}

	if (!pidff->axes_enable) {
		hid_err(pidff->hid, "axes enable field not found\n");
		return -1;
	} else {
		hid_dbg(pidff->hid, "Axes enable count %d logical max %d\n", pidff->axes_enable->report_count, pidff->axes_enable->logical_maximum);
	}

	if (!pidff->device_control) {
		hid_err(pidff->hid, "device control field not found\n");
		return -1;
	}

	if (!pidff->block_load_status && IS_DEVICE_MANAGED(pidff)) {
		hid_err(pidff->hid, "block load status field not found\n");
		return -1;
	}

	if (!pidff->effect_operation_status) {
		hid_err(pidff->hid, "effect operation field not found\n");
		return -1;
	}

	pidff_find_special_keys(pidff->control_id, pidff->device_control,
				pidff_device_control,
				sizeof(pidff_device_control));

	PIDFF_FIND_SPECIAL_KEYS(control_id, device_control, device_control);

	if (IS_DEVICE_MANAGED(pidff)) {
		if (!PIDFF_FIND_SPECIAL_KEYS(type_id, create_new_effect_type,
						effect_types)) {
			hid_err(pidff->hid, "no effect types found\n");
			return -1;
		}

		if (PIDFF_FIND_SPECIAL_KEYS(status_id, block_load_status,
						block_load_status) !=
				sizeof(pidff_block_load_status)) {
			hid_err(pidff->hid,
				"block load status identifiers not found\n");
			return -1;
		}

	} else {
		if (!PIDFF_FIND_SPECIAL_KEYS(type_id, set_effect_type,
						effect_types)) {
			hid_err(pidff->hid, "no effect types found\n");
			return -1;
		}
	}

	if (PIDFF_FIND_SPECIAL_KEYS(operation_id, effect_operation_status,
				    effect_operation_status) !=
			sizeof(pidff_effect_operation_status)) {
		hid_err(pidff->hid, "effect operation identifiers not found\n");
		return -1;
	}

	return 0;
}

/**
 * Find the implemented effect types
 */
static int pidff_find_effects(struct pidff_device *pidff,
			      struct input_dev *dev)
{
	int i;

	if (IS_DEVICE_MANAGED(pidff)) {
		for (i = 0; i < sizeof(pidff_effect_types); i++) {
			int pidff_type = pidff->type_id[i];

			if (pidff->set_effect_type->usage[pidff_type].hid !=
				pidff->create_new_effect_type->
						usage[pidff_type].hid) {
				hid_err(pidff->hid,
					"effect type number %d is invalid\n",
					i);
				return -1;
			}
		}
	}

	if (pidff->type_id[PID_CONSTANT])
		set_bit(FF_CONSTANT, dev->ffbit);
	if (pidff->type_id[PID_RAMP])
		set_bit(FF_RAMP, dev->ffbit);
	if (pidff->type_id[PID_SQUARE]) {
		set_bit(FF_SQUARE, dev->ffbit);
		set_bit(FF_PERIODIC, dev->ffbit);
	}
	if (pidff->type_id[PID_SINE]) {
		set_bit(FF_SINE, dev->ffbit);
		set_bit(FF_PERIODIC, dev->ffbit);
	}
	if (pidff->type_id[PID_TRIANGLE]) {
		set_bit(FF_TRIANGLE, dev->ffbit);
		set_bit(FF_PERIODIC, dev->ffbit);
	}
	if (pidff->type_id[PID_SAW_UP]) {
		set_bit(FF_SAW_UP, dev->ffbit);
		set_bit(FF_PERIODIC, dev->ffbit);
	}
	if (pidff->type_id[PID_SAW_DOWN]) {
		set_bit(FF_SAW_DOWN, dev->ffbit);
		set_bit(FF_PERIODIC, dev->ffbit);
	}
	if (pidff->type_id[PID_SPRING])
		set_bit(FF_SPRING, dev->ffbit);
	if (pidff->type_id[PID_DAMPER])
		set_bit(FF_DAMPER, dev->ffbit);
	if (pidff->type_id[PID_INERTIA])
		set_bit(FF_INERTIA, dev->ffbit);
	if (pidff->type_id[PID_FRICTION])
		set_bit(FF_FRICTION, dev->ffbit);

	return 0;

}

#define PIDFF_FIND_FIELDS(name, report, strict, dev) \
	pidff_find_fields(pidff->name, pidff_ ## name, \
		pidff->reports[report], \
		sizeof(pidff_ ## name), strict, dev)

/*
 * Fill and check the pidff_usages
 */
static int pidff_init_fields(struct pidff_device *pidff, struct input_dev *dev)
{
	int envelope_ok = 0;

	if (PIDFF_FIND_FIELDS(set_effect, PID_SET_EFFECT, 1, pidff)) {
		hid_err(pidff->hid, "unknown set_effect report layout\n");
		return -ENODEV;
	}
	PIDFF_FIND_FIELDS(set_effect_optional, PID_SET_EFFECT, 0, pidff);

	/* Check block offsets TODO: Currently hard coded 2 axis */
	if (!pidff->block_offset[0].field || !pidff->block_offset[1].field) {
		hid_err(pidff->hid, "unknown set_effect report layout (type spec. block offsets)\n");
		return -ENODEV;
	}

	if (PIDFF_FIND_FIELDS(effect_operation, PID_EFFECT_OPERATION,
					1, pidff)) {
		hid_err(pidff->hid, "unknown effect_operation report layout\n");
		return -ENODEV;
	}

	if (!PIDFF_FIND_FIELDS(set_envelope, PID_SET_ENVELOPE, 1, pidff))
		envelope_ok = 1;

	if (pidff_find_special_fields(pidff) || pidff_find_effects(pidff, dev))
		return -ENODEV;

	if (!envelope_ok) {
		if (test_and_clear_bit(FF_CONSTANT, dev->ffbit))
			hid_warn(pidff->hid,
				 "has constant effect but no envelope\n");
		if (test_and_clear_bit(FF_RAMP, dev->ffbit))
			hid_warn(pidff->hid,
				 "has ramp effect but no envelope\n");

		if (test_and_clear_bit(FF_PERIODIC, dev->ffbit))
			hid_warn(pidff->hid,
				 "has periodic effect but no envelope\n");
	}

	if (test_bit(FF_CONSTANT, dev->ffbit) &&
	    PIDFF_FIND_FIELDS(set_constant, PID_SET_CONSTANT, 1, pidff)) {
		hid_warn(pidff->hid, "unknown constant effect layout\n");
		clear_bit(FF_CONSTANT, dev->ffbit);
	}

	if (test_bit(FF_RAMP, dev->ffbit) &&
	    PIDFF_FIND_FIELDS(set_ramp, PID_SET_RAMP, 1, pidff)) {
		hid_warn(pidff->hid, "unknown ramp effect layout\n");
		clear_bit(FF_RAMP, dev->ffbit);
	}

	if ((test_bit(FF_SPRING, dev->ffbit) ||
	     test_bit(FF_DAMPER, dev->ffbit) ||
	     test_bit(FF_FRICTION, dev->ffbit) ||
	     test_bit(FF_INERTIA, dev->ffbit)) &&
	    PIDFF_FIND_FIELDS(set_condition, PID_SET_CONDITION, 1, pidff)) {
		hid_warn(pidff->hid, "unknown condition effect layout\n");
		clear_bit(FF_SPRING, dev->ffbit);
		clear_bit(FF_DAMPER, dev->ffbit);
		clear_bit(FF_FRICTION, dev->ffbit);
		clear_bit(FF_INERTIA, dev->ffbit);
	}

	if (test_bit(FF_PERIODIC, dev->ffbit) &&
	    PIDFF_FIND_FIELDS(set_periodic, PID_SET_PERIODIC, 1, pidff)) {
		hid_warn(pidff->hid, "unknown periodic effect layout\n");
		clear_bit(FF_PERIODIC, dev->ffbit);
	}

	PIDFF_FIND_FIELDS(pool, PID_POOL, 0, pidff);

	if (!PIDFF_FIND_FIELDS(device_gain, PID_DEVICE_GAIN, 1, pidff))
		set_bit(FF_GAIN, dev->ffbit);

	if (IS_DEVICE_MANAGED(pidff)) {
		PIDFF_FIND_FIELDS(block_load, PID_BLOCK_LOAD, 0, pidff);
		if (!pidff->block_load[PID_EFFECT_BLOCK_INDEX].value) {
			hid_err(pidff->hid, "unknown pid_block_load report layout\n");
			return -ENODEV;
		}

		if (PIDFF_FIND_FIELDS(block_free, PID_BLOCK_FREE, 1, pidff)) {
			hid_err(pidff->hid, "unknown pid_block_free report layout\n");
			return -ENODEV;
		}
	}

	if (test_bit(PID_SUPPORTS_POOL_MOVE, &pidff->flags)) {
		if (PIDFF_FIND_FIELDS(pool_move, PID_POOL_MOVE, 0, pidff)) {
			hid_err(pidff->hid, "unknown pid_pool_move report layout\n");
			return -ENODEV;
		}
	}

	return 0;
}

/*
 * Reset the device
 */
static void pidff_reset(struct pidff_device *pidff)
{
	struct hid_device *hid = pidff->hid;

	pidff_empty_memory(pidff);

	pidff->device_control->value[0] = pidff->control_id[PID_RESET];
	/* We reset twice as sometimes hid_wait_io isn't waiting long enough */
	hid_hw_request(hid, pidff->reports[PID_DEVICE_CONTROL], HID_REQ_SET_REPORT);
	hid_hw_wait(hid);
	hid_hw_request(hid, pidff->reports[PID_DEVICE_CONTROL], HID_REQ_SET_REPORT);
	hid_hw_wait(hid);

	pidff->device_control->value[0] =
		pidff->control_id[PID_ENABLE_ACTUATORS];
	hid_hw_request(hid, pidff->reports[PID_DEVICE_CONTROL], HID_REQ_SET_REPORT);
	hid_hw_wait(hid);
}

/*
 * Test if autocenter modification is using the supported method
 */
static int pidff_check_autocenter(struct pidff_device *pidff,
				  struct input_dev *dev)
{
	int error;
	/*struct ff_effect autocenter; */

	if (IS_DEVICE_MANAGED(pidff)) {
		/*
		* Let's find out if autocenter modification is supported
		* Specification doesn't specify anything, so we request an
		* effect upload and cancel it immediately. If the approved
		* effect id was one above the minimum, then we assume the first
		* effect id is a built-in spring type effect used for autocenter
		*/

		error = pidff_request_effect_upload(pidff, 1);
		if (error) {
			hid_err(pidff->hid, "upload request failed\n");
			return error;
		}

		if (pidff->active.id ==
				pidff->block_load[PID_EFFECT_BLOCK_INDEX].
				field->logical_minimum + 1) {
			pidff_autocenter(pidff, 0xffff);
			set_bit(FF_AUTOCENTER, dev->ffbit);
		} else {
			hid_notice(pidff->hid,
				"device has unknown autocenter control method\n");
		}

		pidff_erase_pid(pidff, pidff->active.id);
	}

	/*
	 * In driver managed mode, there is no way of knowing if a
	 * pre-configured spring effect exists or not...
	 */

	return 0;
}

/*
 * Do initialization that requires hw requests
 */
static void pidff_init_hw_requests(struct pidff_device *pidff, struct input_dev *dev)
{
	int i = 0;

	/* pool report is sometimes messed up, refetch it */
	hid_hw_request(pidff->hid, pidff->reports[PID_POOL],
			HID_REQ_GET_REPORT);
	hid_hw_wait(pidff->hid);

	if (pidff->pool[PID_SIMULTANEOUS_MAX].value) {
		while (pidff->pool[PID_SIMULTANEOUS_MAX].value[0] < 2) {
			if (i++ > 2) {
				hid_notice(pidff->hid,
					 "device reports %d simultaneous effects\n",
					 pidff->pool[PID_SIMULTANEOUS_MAX].value[0]);
				break;
			}
			hid_dbg(pidff->hid, "pid_pool requested again\n");
			hid_hw_request(pidff->hid, pidff->reports[PID_POOL],
					  HID_REQ_GET_REPORT);
			hid_hw_wait(pidff->hid);
		}
	}

	for (i = 0; i < PID_EFFECTS_MAX; i++) {
		pidff->effect[i].id = -1;
		pidff->effect[i].offset[0] = NULL;
		pidff->effect[i].offset[1] = NULL;
	}

	if (pidff->pool[PID_RAM_POOL_SIZE].value &&
			pidff->pool[PID_RAM_POOL_SIZE].value[0] > 0) {
		pidff->pid_total_ram = pidff->pool[PID_RAM_POOL_SIZE].value[0];
	} else if (pidff->block_offset[0].field) {
		pidff->pid_total_ram =
				pidff->block_offset[0].field->logical_maximum
				- pidff->block_offset[0].field->logical_minimum
				+ 1;
	}
	hid_dbg(pidff->hid, "device memory size is %d bytes\n",
			pidff->pid_total_ram);

	if (pidff->pool[PID_POOL_ALIGNMENT].value &&
			pidff->pool[PID_POOL_ALIGNMENT].value[0] > 0)
		pidff->alignment = pidff->pool[PID_POOL_ALIGNMENT].value[0];
	else
		pidff->alignment = 1;
	hid_dbg(pidff->hid, "device memory alignment is %d\n",
			pidff->alignment);

	pidff_report_sizes(pidff);

/*	if (pidff->pool[PID_DEVICE_MANAGED_POOL].value &&
	    pidff->pool[PID_DEVICE_MANAGED_POOL].value[0] == 0) {*/
	if (!pidff->pool[PID_DEVICE_MANAGED_POOL].value ||
	    pidff->pool[PID_DEVICE_MANAGED_POOL].value[0] == 0) {
		hid_dbg(pidff->hid,
			   "device does not support device managed pool\n");

		clear_bit(PID_SUPPORTS_DEVICE_MANAGED, &pidff->flags);
	}

	if (pidff->max_effects > PID_EFFECTS_MAX)
		pidff->max_effects = PID_EFFECTS_MAX;

	if (pidff->pool[PID_SIMULTANEOUS_MAX].value)
		hid_notice(pidff->hid, "max simultaneous effects is %d\n",
			pidff->pool[PID_SIMULTANEOUS_MAX].value[0]);

	if (test_bit(FF_GAIN, dev->ffbit)) {
		pidff_set(&pidff->device_gain[PID_DEVICE_GAIN_FIELD], 0xffff);
		hid_hw_request(pidff->hid, pidff->reports[PID_DEVICE_GAIN],
				     HID_REQ_SET_REPORT);
	}

	pidff_check_autocenter(pidff, dev);
}

/*
 * Check if the device is PID and initialize it
 */
int hid_pidff_init(struct hid_device *hid)
{
	struct pidff_device *pidff;
	struct hid_input *hidinput = list_entry(hid->inputs.next,
						struct hid_input, list);
	struct input_dev *dev = hidinput->input;
	struct ff_device *ff;
	int error;

	hid_dbg(hid, "starting pid init\n");

	if (list_empty(&hid->report_enum[HID_OUTPUT_REPORT].report_list)) {
		hid_dbg(hid, "not a PID device, no output report\n");
		return -ENODEV;
	}

	pidff = kzalloc(sizeof(*pidff), GFP_KERNEL);
	if (!pidff)
		return -ENOMEM;

	INIT_LIST_HEAD(&pidff->memory);

	pidff->hid = hid;
	pidff->flags = 0xff;	/* Check support later */
	pidff->active.id = 0;

	hid_device_io_start(hid);

	pidff_find_reports(hid, HID_OUTPUT_REPORT, pidff);
	pidff_find_reports(hid, HID_FEATURE_REPORT, pidff);

	if (!pidff_reports_ok(pidff)) {
		hid_dbg(hid, "reports not ok, aborting\n");
		error = -ENODEV;
		goto fail;
	}

	error = pidff_init_fields(pidff, dev);
	if (error)
		goto fail;

	pidff_reset(pidff);

	/* Do the initialization part which requires hw requests */
	pidff_init_hw_requests(pidff, dev);

	/* Determine max effects, this one should work with device managed devices */
	if (pidff->block_load[PID_EFFECT_BLOCK_INDEX].field) {
		pidff->max_effects =
			pidff->block_load[PID_EFFECT_BLOCK_INDEX].field->logical_maximum -
			pidff->block_load[PID_EFFECT_BLOCK_INDEX].field->logical_minimum +
				1;
	}

	/* And if it fails, this one should work with driver managed */
	if (!pidff->max_effects &&
			pidff->set_effect[PID_EFFECT_BLOCK_INDEX].field) {
		pidff->max_effects =
			pidff->set_effect[PID_EFFECT_BLOCK_INDEX].field->logical_maximum -
			pidff->set_effect[PID_EFFECT_BLOCK_INDEX].field->logical_minimum +
				1;
	}
	hid_dbg(pidff->hid, "device max effects %d\n", pidff->max_effects);

	error = input_ff_create(dev, pidff->max_effects);
	if (error)
		goto fail;

	ff = dev->ff;
	ff->private = pidff;
	ff->upload = pidff_upload_effect;
	ff->erase = pidff_erase_effect;
	ff->set_gain = pidff_set_gain;
	ff->set_autocenter = pidff_set_autocenter;
	ff->playback = pidff_playback;

	hid_info(dev, "Force feedback for USB HID PID devices by Anssi Hannula <anssi.hannula@gmail.com>\n");

	hid_device_io_stop(hid);

	return 0;

 fail:
	pidff_empty_memory(pidff);
	hid_device_io_stop(hid);

	kfree(pidff);
	return error;
}

/*
 * Remove any allocated memory (for driver managed mode)
 */
void hid_pidff_destroy(struct hid_device *hid)
{
	struct hid_input *hidinput = list_entry(hid->inputs.next,
			struct hid_input, list);
	struct input_dev *dev = hidinput->input;
	struct pidff_device *pidff = dev->ff->private;

	if (pidff)
		pidff_empty_memory(pidff);
}

