<?php

class php_element {
	function is_type($name) {
		$types = array("void"     => "void", 
									 "bool"     => "bool", 
									 "boolean"  => "bool", 
									 "int"      => "int", 
									 "integer"  => "int", 
									 "float"    => "float", 
									 "double"   => "float", 
									 "real"     => "float", 
									 "string"   => "string", 
									 "array"    => "array",
									 "object"   => "object",
									 "resource" => "resource",
									 "mixed"    => "mixed",
									 "callback" => "callback",
									 );
		
		if(isset($types[$name])) {
			return $types[$name];
		} else {
			return false;
		}
	}

	function is_name($name) {
		if(ereg("[a-zA-Z0-9_]",$name)) {
			// TODO reserved words
			return true;
		} 
		return false;
	}


	function c_code() {
		return "";
	}

	function h_code() {
		return "";
	}

	function docbook_xml() {
		return "";
	}

	function docbook_editor_footer($level=3) {
		return '
<!-- Keep this comment at the end of the file
Local variables:
mode: sgml
sgml-omittag:t
sgml-shorttag:t
sgml-minimize-attributes:nil
sgml-always-quote-attributes:t
sgml-indent-step:1
sgml-indent-data:t
indent-tabs-mode:nil
sgml-parent-document:nil
sgml-default-dtd-file:"'.str_repeat("../",$level).'manual.ced"
sgml-exposed-tags:nil
sgml-local-catalogs:nil
sgml-local-ecat-files:nil
End:
vim600: syn=xml fen fdm=syntax fdl=2 si
vim: et tw=78 syn=sgml
vi: ts=1 sw=1
-->
';
	}
}

?>