<?php
class xsl {

	function xsl() {
		$this->_parser = xslt_create();
	}
	
	function set_error() {
		xslt_set_error_handler($this->_parser, array($this, 'xslt_trap_error'));
		echo "OK";
	}
	
	function xslt_trap_error($parser, $errorno, $level, $fields) {
		return TRUE;
	}
	function clean() {
		xslt_free($this->_parser);
	}
}

$x = new xsl;
// work-around for possible '$this does not exist' bug in constructor
$x->set_error();
$x->clean();
?>
