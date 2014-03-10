/* pass 9
 *
 * - optimize usage of temporary variables
 */

if (ZEND_OPTIMIZER_PASS_9 & OPTIMIZATION_LEVEL) {
	optimize_temporary_variables(op_array);
}
