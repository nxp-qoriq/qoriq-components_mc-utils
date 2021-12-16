/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2018, 2020 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 * names of any contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __FSL_DPDMAI_H
#define __FSL_DPDMAI_H

struct fsl_mc_io;

/** @addtogroup dpdmai Data Path DMA Interface API
 * Contains initialization APIs and runtime control APIs for DPDMAI
 * @{
 */

/* General DPDMAI macros */

/**
 * Maximum number of Tx/Rx priorities per DPDMAI object
 */
#define DPDMAI_PRIO_NUM		2

/**
 * All queues considered; see dpdmai_set_rx_queue()
 */
#define DPDMAI_ALL_QUEUES	(uint8_t)(-1)

int dpdmai_open(struct fsl_mc_io *mc_io,
		uint32_t cmd_flags,
		int dpdmai_id,
		uint16_t *token);

int dpdmai_close(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token);

/* DPDMAI options */

/**
 * Enable individual Congestion Groups usage per each priority queue
 * If this option is not enabled then only one CG is used for all priority queues
 * If this option is enabled then a separate specific CG is used for each individual priority queue.
 * In this case the priority queue must be specified via congestion notification API
 */
#define DPDMAI_OPT_CG_PER_PRIORITY		0x00000001

/**
 * struct dpdmai_cfg - Structure representing DPDMAI configuration
 * @priorities: Priorities for the DMA hardware processing; valid priorities are
 *	configured with values 1-8; the entry following last valid entry
 *	should be configured with 0
 *	@options: dpdmai options
 */
struct dpdmai_cfg {
	uint8_t num_queues;
	uint8_t priorities[DPDMAI_PRIO_NUM];
	struct {
		uint32_t options;
	} adv;
};

int dpdmai_create(struct fsl_mc_io *mc_io,
		  uint16_t dprc_token,
		  uint32_t cmd_flags,
		  const struct dpdmai_cfg *cfg,
		  uint32_t *obj_id);

int dpdmai_destroy(struct fsl_mc_io *mc_io,
		   uint16_t dprc_token,
		   uint32_t cmd_flags,
		   uint32_t object_id);

int dpdmai_enable(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  uint16_t token);

int dpdmai_disable(struct fsl_mc_io *mc_io,
		   uint32_t cmd_flags,
		   uint16_t token);

int dpdmai_is_enabled(struct fsl_mc_io *mc_io,
		      uint32_t cmd_flags,
		      uint16_t token,
		      int *en);

int dpdmai_reset(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token);

int dpdmai_set_irq_enable(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint8_t en);

int dpdmai_get_irq_enable(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint8_t *en);

int dpdmai_set_irq_mask(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint32_t mask);

int dpdmai_get_irq_mask(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint32_t *mask);

int dpdmai_get_irq_status(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint32_t *status);

int dpdmai_clear_irq_status(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t irq_index,
			    uint32_t status);

/**
 * struct dpdmai_attr - Structure representing DPDMAI attributes
 * @id: DPDMAI object ID
 * @num_of_priorities: number of priorities
 * @options: dpdmai options
 */
struct dpdmai_attr {
	int id;
	uint8_t num_of_priorities;
	uint8_t num_of_queues;
	uint32_t options;
};

int dpdmai_get_attributes(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  struct dpdmai_attr *attr);

/**
 * enum dpdmai_dest - DPDMAI destination types
 * @DPDMAI_DEST_NONE: Unassigned destination; The queue is set in parked mode
 *	and does not generate FQDAN notifications; user is expected to dequeue
 *	from the queue based on polling or other user-defined method
 * @DPDMAI_DEST_DPIO: The queue is set in schedule mode and generates FQDAN
 *	notifications to the specified DPIO; user is expected to dequeue
 *	from the queue only after notification is received
 * @DPDMAI_DEST_DPCON: The queue is set in schedule mode and does not generate
 *	FQDAN notifications, but is connected to the specified DPCON object;
 *	user is expected to dequeue from the DPCON channel
 */
enum dpdmai_dest {
	DPDMAI_DEST_NONE = 0,
	DPDMAI_DEST_DPIO = 1,
	DPDMAI_DEST_DPCON = 2
};

/**
 * struct dpdmai_dest_cfg - Structure representing DPDMAI destination parameters
 * @dest_type: Destination type
 * @dest_id: Either DPIO ID or DPCON ID, depending on the destination type
 * @priority: Priority selection within the DPIO or DPCON channel; valid values
 *	are 0-1 or 0-7, depending on the number of priorities in that
 *	channel; not relevant for 'DPDMAI_DEST_NONE' option
 */
struct dpdmai_dest_cfg {
	enum dpdmai_dest dest_type;
	int dest_id;
	uint8_t priority;
};

/* DPDMAI queue modification options */

/**
 * Select to modify the user's context associated with the queue
 */
#define DPDMAI_QUEUE_OPT_USER_CTX	0x00000001

/**
 * Select to modify the queue's destination
 */
#define DPDMAI_QUEUE_OPT_DEST		0x00000002

/**
 * struct dpdmai_rx_queue_cfg - DPDMAI RX queue configuration
 * @options: Flags representing the suggested modifications to the queue;
 *	Use any combination of 'DPDMAI_QUEUE_OPT_<X>' flags
 * @user_ctx: User context value provided in the frame descriptor of each
 *	dequeued frame;
 *	valid only if 'DPDMAI_QUEUE_OPT_USER_CTX' is contained in 'options'
 * @dest_cfg: Queue destination parameters;
 *	valid only if 'DPDMAI_QUEUE_OPT_DEST' is contained in 'options'
 */
struct dpdmai_rx_queue_cfg {
	uint32_t options;
	uint64_t user_ctx;
	struct dpdmai_dest_cfg dest_cfg;

};

int dpdmai_set_rx_queue(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t queue_idx,
			uint8_t priority,
			const struct dpdmai_rx_queue_cfg *cfg);

/**
 * struct dpdmai_rx_queue_attr - Structure representing attributes of Rx queues
 * @user_ctx:  User context value provided in the frame descriptor of each
 *	 dequeued frame
 * @dest_cfg: Queue destination configuration
 * @fqid: Virtual FQID value to be used for dequeue operations
 */
struct dpdmai_rx_queue_attr {
	uint64_t user_ctx;
	struct dpdmai_dest_cfg dest_cfg;
	uint32_t fqid;
};

int dpdmai_get_rx_queue(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t queue_idx,
			uint8_t priority,
			struct dpdmai_rx_queue_attr *attr);

/**
 * struct dpdmai_tx_queue_attr - Structure representing attributes of Tx queues
 * @fqid: Virtual FQID to be used for sending frames to DMA hardware
 */

struct dpdmai_tx_queue_attr {
	uint32_t fqid;
};

int dpdmai_get_tx_queue(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t queue_idx,
			uint8_t priority,
			struct dpdmai_tx_queue_attr *attr);

/**
 * enum dpdmai_congestion_unit - DPDMAI congestion unit types
 * @DPDMAI_CONGESTION_UNIT_BYTES: Use bytes as congestion units
 * @DPDMAI_CONGESTION_UNIT_FRAMES: Use frames as congestion units
 */
enum dpdmai_congestion_unit {
	DPDMAI_CONGESTION_UNIT_BYTES = 0,
	DPDMAI_CONGESTION_UNIT_FRAMES
};

/**
 * CSCN message is written to message_iova once entering a
 * congestion state (see 'threshold_entry')
 */
#define DPDMAI_CGN_MODE_WRITE_MEM_ON_ENTER	0x00000001
/**
 * CSCN message is written to message_iova once exiting a
 * congestion state (see 'threshold_exit')
 */
#define DPDMAI_CGN_MODE_WRITE_MEM_ON_EXIT	0x00000002
/**
 * CSCN write will attempt to allocate into a cache (coherent write);
 * valid only if 'DPDMAI_CGN_MODE_WRITE_MEM_<X>' is selected
 */
#define DPDMAI_CGN_MODE_COHERENT_WRITE		0x00000004

/**
 * struct dpdmai_congestion_notification_cfg - congestion notification
 *		configuration
 * @units: units type
 * @notification_mode: Mask of available options; use 'DPDMAI_CGN_MODE_<X>'
 *		values
 * @threshold_entry: above this threshold we enter a congestion state.
 *		set it to '0' to disable it
 * @threshold_exit: below this threshold we exit the congestion state.
 * @message_ctx: The context that will be part of the CSCN message
 * @message_iova: I/O virtual address (must be in DMA-able memory),
 *		must be 16B aligned;
 */
struct dpdmai_congestion_notification_cfg {
	enum dpdmai_congestion_unit units;
	uint16_t notification_mode;
	uint32_t threshold_entry;
	uint32_t threshold_exit;
	uint64_t message_ctx;
	uint64_t message_iova;
};

int dpdmai_set_rx_congestion_notification(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t priority,
			struct dpdmai_congestion_notification_cfg *cfg);

int dpdmai_set_tx_congestion_notification(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t priority,
			struct dpdmai_congestion_notification_cfg *cfg);

int dpdmai_get_rx_congestion_notification(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t priority,
			struct dpdmai_congestion_notification_cfg *cfg);

int dpdmai_get_tx_congestion_notification(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t priority,
			struct dpdmai_congestion_notification_cfg *cfg);

int dpdmai_get_api_version(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t *major_ver,
			   uint16_t *minor_ver);

#endif /* __FSL_DPDMAI_H */
