/*
 * Argon2 source code package
 *
 * Written by Daniel Dinu and Dmitry Khovratovich, 2015
 *
 * This work is licensed under a Creative Commons CC0 1.0 License/Waiver.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with
 * this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef ARGON2_REF_H
#define ARGON2_REF_H

#include "core.h"

/*
 * Function fills a new memory block by XORing over @next_block. @next_block must be initialized
 * @param prev_block Pointer to the previous block
 * @param ref_block Pointer to the reference block
 * @param next_block Pointer to the block to be constructed
 * @pre all block pointers must be valid
 */
void fill_block_with_xor(const block *prev_block, const block *ref_block,
                block *next_block);

/* LEGACY CODE: version 1.2.1 and earlier
* Function fills a new memory block by overwriting @next_block. 
* @param prev_block Pointer to the previous block
* @param ref_block Pointer to the reference block
* @param next_block Pointer to the block to be constructed
* @pre all block pointers must be valid
*/
void fill_block(const block *prev_block, const block *ref_block,
    block *next_block);

/*
 * Generate pseudo-random values to reference blocks in the segment and puts
 * them into the array
 * @param instance Pointer to the current instance
 * @param position Pointer to the current position
 * @param pseudo_rands Pointer to the array of 64-bit values
 * @pre pseudo_rands must point to @a instance->segment_length allocated values
 */
void generate_addresses(const argon2_instance_t *instance,
                        const argon2_position_t *position,
                        uint64_t *pseudo_rands);

#endif /* ARGON2_REF_H */
