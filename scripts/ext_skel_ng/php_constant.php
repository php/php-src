<?php

	class php_constant extends php_element {
		function php_constant($name, $value, $type="string", $desc="") {
			$this->name = $name;
			$this->value= $value;
			$this->type = $type;
			$this->desc = $desc;
		} 
		
		function c_code() {
			switch($this->type) {
			case "integer":
				return "REGISTER_LONG_CONSTANT(\"{$this->name}\", {$this->value}, 0);\n";
			
			case "float":
				return "REGISTER_DOUBLE_CONSTANT(\"{$this->name}\", {$this->value}, 0);\n";

			case "string":
				return "REGISTER_STRING_CONSTANT(\"{$this->name}\", \"$value\", ".strlen($this->value).", 0);\n";
			}
		}

		function docbook_xml() {
			return trim("
<row>
 <entry>
  <constant id='constant".strtolower(str_replace("_","-",$this->name))."'>$name</constant>
  (<link linkend='language.types.integer'>integer</link>)
 </entry>
 <entry>{$this->value}</entry>
 <entry>{$this->desc}</entry>
</row>
")."\n";
		}
	}

?>