<?php

	class php_ini extends php_element {

		function __construct($attr) {
			
			$this->name  = $attr["name"];
	
			if (!$this->is_name($this->name)) {
				$this->error[] = "'$attr[name]' is not a valid php.ini directive name";
			}

			$this->type  = $this->is_type($attr['type']);
      if (!$this->type) {
        $this->error[] = "'$attr[type]' is not a valid PHP data type";
      }

			$this->value = $attr["value"];
			$this->desc  = $attr["desc"];

			switch (@$attr["access"]) {
			case "system":
				$this->access = "PHP_INI_SYSTEM";
				break;
			case "perdir":
				$this->access = "PHP_INI_PERDIR";
				break;
			case "user":
				$this->access = "PHP_INI_USER";
				break;
			case "all":
			case "":
				$this->access = "PHP_INI_ALL";
				break;
			default:
				$this->error[] = "'$attr[access]' is not a valid access mode (system|perdir|user|all)";
			}

			switch ($this->type) {
			case "bool":
				$this->onupdate = "OnUpdateBool";
				$this->c_type = "zend_bool";
				break;					
			case "int":
				$this->onupdate = "OnUpdateLong";
				$this->c_type = "long";
				break;
			case "float":
				$this->onupdate = "OnUpdateReal";
				$this->c_type = "double";
				break;
			case "string":
				$this->onupdate = "OnUpdateString";
				$this->c_type = "char *";
				break;
			default:
				$this->error[] = "'$this->type' not supported, only bool, int, float and string"; 
				break;
			}

			if (isset($attr["onupdate"])) {
				$this->onupdate = $attr["onupdate"];
			} 
		} 
		


		static function c_code_header($name) {
			return "PHP_INI_BEGIN()\n";
		}

		function c_code($name) {
			return "  STD_PHP_INI_ENTRY(\"$name.{$this->name}\", \"{$this->value}\", {$this->access}, {$this->onupdate}, {$this->name}, zend_{$name}_globals, {$name}_globals)\n";
		}

		static function c_code_footer() {
			return "PHP_INI_END()\n\n";
		}



		static function docbook_xml_header($name) {
			return
"    <table>
     <title>$name runtime configuration</title>
			<tgroup cols='3'>
			 <thead>
        <row>
         <entry>directive</entry>
         <entry>default value</entry>
         <entry>descrpition</entry>
        </row>
       </thead>
      <tbody>
";
		}

		function docbook_xml() {
			return 
"    <row>
     <entry>$this->name</entry>
     <entry>$this->value</entry>
     <entry>$this->desc</entry>
    </row>
";
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
