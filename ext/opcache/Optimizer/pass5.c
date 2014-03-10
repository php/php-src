if (ZEND_OPTIMIZER_PASS_5 & OPTIMIZATION_LEVEL) {
	zend_block_optimization(op_array TSRMLS_CC);
}
