<?php

	class php_constant extends php_element {

		function __construct($attr, $desc) {

			$this->name = $attr["name"];
			if (!$this->is_name($this->name)) {
				$this->error[] = "'$attr[name]'is not a valid constant name";
			}

			$this->type = isset($attr["type"]) ? $this->is_type($attr["type"]) : "string";
			if (!in_array($this->type, array('int', 'float', 'string'))) {
				$this->error[] = "'$attr[type]' is not a valid constant type, only int, float and string";
			} 

			$this->value= $attr["value"];
			$this->desc = $desc;
		} 


		static function c_code_header($name) {
			return "";
		}
		
		function c_code() {
			switch ($this->type) {
			case "int":
				return "REGISTER_LONG_CONSTANT(\"{$this->name}\", {$this->value}, 0);\n";
			
			case "float":
				return "REGISTER_DOUBLE_CONSTANT(\"{$this->name}\", {$this->value}, 0);\n";

			case "string":
				return "REGISTER_STRING_CONSTANT(\"{$this->name}\", \"$value\", ".strlen($this->value).", 0);\n";
			}
		}

		static function c_code_footer() {
			return "";
		}

		
		
		static function docbook_xml_header($name) {
			return
"    <table>
     <title>$name constants</title>
      <tgroup cols='3'>
       <thead>
        <row>
         <entry>name</entry>
         <entry>value</entry>
         <entry>descrpition</entry>
        </row>
       </thead>
      <tbody>
";
		}
			
		function docbook_xml() {
			return trim("
<row>
 <entry>
  <constant id='constant".strtolower(str_replace("_","-",$this->name))."'>{$this->name}</constant>
  (<link linkend='language.types.integer'>integer</link>)
 </entry>
 <entry>{$this->value}</entry>
 <entry>{$this->desc}</entry>
</row>
")."\n";
		}

		static function docbook_xml_footer() {
			return
"     </tbody>
    </tgroup>
   </table>
";
		}
	}

?>
