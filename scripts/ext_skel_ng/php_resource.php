<?php

	class php_resource extends php_element {
		function php_resource($name, $payload, $destruct, $description) {
			$this->name = $name;
			$this->payload = $payload;
			$this->destruct = $destruct;
			$this->description = $description;

			if (empty($this->destruct) && strstr($this->payload, "*")) {
				$this->destruct = "  free(resource);\n";
			}

			if(empty($this->payload)) {
				$this->payload = "int";
			}
		} 
		
		function docbook_xml($base) {
			return "
    <section id='$base.resources.{$this->name}'>
     <title><litera>{$this->name}</literal></title>
     <para>
      {$this->description}
     </para>
    </section>
";
		}

		function minit_code() {
			return "
le_{$this->name} = zend_register_list_destructors_ex({$this->name}_dtor, 
                                                     NULL, 
                                                     \"{$this->name}\", 
                                                     module_number);

";
		}

		function c_code() {
			return "
int le_{$this->name};

void {$this->name}_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
  {$this->payload} resource = ({$this->payload})(rsrc->ptr);

  {$this->destruct}
}
";
		}

		function h_code() {
      $upname = strtoupper($this->name);

			return "
#define {$upname}_FETCH(r, z)   ZEND_FETCH_RESOURCE(r, {$this->payload}, z, -1, ${$this->name}, le_{$this->name }); \
                                    if(!r) { RETURN_FALSE; }

#define {$upname}_REGISTER(r)   ZEND_REGISTER_RESOURCE(return_value, r, le_{$this->name });
";
		}
	}

?>