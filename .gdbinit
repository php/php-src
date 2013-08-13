define set_ts
	set $tsrm_ls = $arg0
end

document set_ts
	set the ts resource, it is impossible for gdb to
	call ts_resource_ex while no process is running,
	but we could get the resource from the argument 
	of frame info.
end

define ____executor_globals
	if basic_functions_module.zts
		if !$tsrm_ls
			set $tsrm_ls = ts_resource_ex(0, 0)
		end
		set $eg = ((zend_executor_globals*) (*((void ***) $tsrm_ls))[executor_globals_id-1])
		set $cg = ((zend_compiler_globals*) (*((void ***) $tsrm_ls))[compiler_globals_id-1])
	else
		set $eg = executor_globals
		set $cg = compiler_globals
	end
end

document ____executor_globals
	portable way of accessing executor_globals, set $eg
	this also sets compiler_globals to $cg
	ZTS detection is automatically based on ext/standard module struct
end

define print_cvs
	____executor_globals
	set $p = $eg.current_execute_data.CVs
	set $c = $eg.current_execute_data.op_array.last_var
	set $v = $eg.current_execute_data.op_array.vars
	set $i = 0

	printf "Compiled variables count: %d\n", $c
	while $i < $c
		printf "%d = %s\n", $i, $v[$i].name
		if $p[$i] != 0
			printzv *$p[$i]
		else
			printf "*uninitialized*\n"
		end
		set $i = $i + 1
	end
end

define dump_bt
	set $t = $arg0
	while $t
		printf "[%p] ", $t
		set $fst = $t->function_state
		if $fst.function->common.function_name
			if $fst.arguments
				set $count = (int)*($fst.arguments)

				if $t->object
					if $fst.function.common.scope
						printf "%s->", $fst.function.common.scope->name
					else
						if !$eg
							____executor_globals
						end

						set $known_class = 0
						if $eg
							set $handle = $t->object.value.obj.handle
							set $handlers = $t->object.value.obj.handlers
							set $zobj = (zend_object *)$eg.objects_store.object_buckets[$handle].bucket.obj.object

							if $handlers->get_class_entry == &zend_std_object_get_class
								set $known_class = 1

								if $handlers.get_class_name
									if $handlers.get_class_name != &zend_std_object_get_class_name
										set $known_class = 0
									end
								end

								if $known_class
									printf "%s->", $zobj->ce.name
								end
							end
						end

						if !$known_class
							printf "(Unknown)->"
						end
					end
				else
					if $fst.function.common.scope
						printf "%s::", $fst.function.common.scope->name
					end
				end

				printf "%s(", $fst.function->common.function_name
				while $count > 0
					set $zvalue = *(zval **)($fst.arguments - $count)
					set $type = $zvalue->type
					if $type == 0
						printf "NULL"
					end
					if $type == 1
						printf "%ld", $zvalue->value.lval
					end
					if $type == 2
						printf "%f", $zvalue->value.dval
					end
					if $type == 3
						if $zvalue->value.lval
							printf "true"
						else
							printf "false"
						end
					end
					if $type == 4
						printf "array(%d)[%p]", $zvalue->value.ht->nNumOfElements, $zvalue
					end
					if $type == 5
						printf "object[%p]", $zvalue
					end
					if $type == 6
						____print_str $zvalue->value.str.val $zvalue->value.str.len
					end
					if $type == 7
						printf "resource(#%d)", $zvalue->value.lval
					end
					if $type == 8
						printf "constant"
					end
					if $type == 9
						printf "const_array"
					end
					if $type > 9
						printf "unknown type %d", $type
					end
					set $count = $count -1
					if $count > 0
						printf ", "
					end
				end
				printf ") "
			else
				printf "%s() ", $fst.function->common.function_name
			end
		else
			printf "??? "
		end
		if $t->op_array != 0
			printf "%s:%d ", $t->op_array->filename, $t->opline->lineno
		end
		set $t = $t->prev_execute_data
		printf "\n"
	end
end

document dump_bt
	dumps the current execution stack. usage: dump_bt executor_globals.current_execute_data
end

define printzv
	set $ind = 1
	____printzv $arg0 0 
end

document printzv
	prints zval contents
end

define ____printzv_contents
	set $zvalue = $arg0
	set $type = $zvalue->type

	printf "(refcount=%d", $zvalue->refcount__gc
	if $zvalue->is_ref__gc
		printf ",is_ref"
	end
	printf ") "
	if $type == 0
		printf "NULL"
	end
	if $type == 1
		printf "long: %ld", $zvalue->value.lval
	end
	if $type == 2
		printf "double: %f", $zvalue->value.dval
	end
	if $type == 3
		printf "bool: "
		if $zvalue->value.lval
			printf "true"
		else
			printf "false"
		end
	end
	if $type == 4
		printf "array(%d): ", $zvalue->value.ht->nNumOfElements
		if ! $arg1
			printf "{\n"
			set $ind = $ind + 1
			____print_ht $zvalue->value.ht 1
			set $ind = $ind - 1
			set $i = $ind
			while $i > 0
				printf "  "
				set $i = $i - 1
			end
			printf "}"
		end
		set $type = 0
	end
	if $type == 5
		printf "object"
		____executor_globals
		set $handle = $zvalue->value.obj.handle
		set $handlers = $zvalue->value.obj.handlers
		if basic_functions_module.zts
			set $zobj = zend_objects_get_address($zvalue, $tsrm_ls)
		else
			set $zobj = zend_objects_get_address($zvalue)
		end
		if $handlers->get_class_entry == &zend_std_object_get_class
			set $cname = $zobj->ce.name
		else
			set $cname = "Unknown"
		end
		printf "(%s) #%d", $cname, $handle
		if ! $arg1
			if $handlers->get_properties == &zend_std_get_properties
				set $ht = $zobj->properties
				if $ht
					printf "(%d): ", $ht->nNumOfElements
					printf "{\n"
					set $ind = $ind + 1
					____print_ht $ht 1
					set $ind = $ind - 1
					set $i = $ind
					while $i > 0
						printf "  "
						set $i = $i - 1
					end
					printf "}"
				else
					echo "no properties found"
				end
			end
		end
		set $type = 0
	end
	if $type == 6
		printf "string(%d): ", $zvalue->value.str.len
		____print_str $zvalue->value.str.val $zvalue->value.str.len
	end
	if $type == 7
		printf "resource: #%d", $zvalue->value.lval
	end
	if $type == 8 
		printf "constant"
	end
	if $type == 9
		printf "const_array"
	end
	if $type > 9
		printf "unknown type %d", $type
	end
	printf "\n"
end

define ____printzv
	____executor_globals
	set $zvalue = $arg0

	printf "[%p] ", $zvalue

	if $zvalue == $eg.uninitialized_zval_ptr
		printf "*uninitialized* "
	end

	set $zcontents = (zval*) $zvalue
	if $arg1
		____printzv_contents $zcontents $arg1
	else
		____printzv_contents $zcontents 0 
	end
end

define ____print_const_table
	set $ht = $arg0
	set $p = $ht->pListHead

	while $p != 0
		set $const = (zend_constant *) $p->pData

		set $i = $ind
		while $i > 0
			printf "  "
			set $i = $i - 1
		end

		if $p->nKeyLength > 0
			____print_str $p->arKey $p->nKeyLength
			printf " => "
		else
			printf "%d => ", $p->h
		end

		____printzv_contents &$const->value 0
		set $p = $p->pListNext
	end
end

define print_const_table
	set $ind = 1
	printf "[%p] {\n", $arg0
	____print_const_table $arg0
	printf "}\n"
end

define ____print_ht
	set $ht = (HashTable*)$arg0
	set $p = $ht->pListHead

	while $p != 0
		set $i = $ind
		while $i > 0
			printf "  "
			set $i = $i - 1
		end

		if $p->nKeyLength > 0
			____print_str $p->arKey $p->nKeyLength
			printf " => "
		else
			printf "%d => ", $p->h
		end
		
		if $arg1 == 0
			printf "%p\n", (void*)$p->pData
		end
		if $arg1 == 1
			set $zval = *(zval **)$p->pData
			____printzv $zval 1
		end
		if $arg1 == 2
			printf "%s\n", (char*)$p->pData
		end

		set $p = $p->pListNext
	end
end

define print_ht
	set $ind = 1
	printf "[%p] {\n", $arg0
	____print_ht $arg0 1
	printf "}\n"
end

document print_ht
	dumps elements of HashTable made of zval
end

define print_htptr
	set $ind = 1
	printf "[%p] {\n", $arg0
	____print_ht $arg0 0
	printf "}\n"
end

document print_htptr
	dumps elements of HashTable made of pointers
end

define print_htstr
	set $ind = 1
	printf "[%p] {\n", $arg0
	____print_ht $arg0 2
	printf "}\n"
end

document print_htstr
	dumps elements of HashTable made of strings
end

define ____print_ft
	set $ht = $arg0
	set $p = $ht->pListHead

	while $p != 0
		set $func = (zend_function*)$p->pData

		set $i = $ind
		while $i > 0
			printf "  "
			set $i = $i - 1
		end

		if $p->nKeyLength > 0
			____print_str $p->arKey $p->nKeyLength
			printf " => "
		else
			printf "%d => ", $p->h
		end

		printf "\"%s\"\n", $func->common.function_name
		set $p = $p->pListNext
	end
end

define print_ft
	set $ind = 1
	printf "[%p] {\n", $arg0
	____print_ft $arg0
	printf "}\n"
end

document print_ft
	dumps a function table (HashTable)
end

define ____print_inh_class
	set $ce = $arg0
	if $ce->ce_flags & 0x10 || $ce->ce_flags & 0x20
		printf "abstract "
	else
		if $ce->ce_flags & 0x40
			printf "final "
		end
	end
	printf "class %s", $ce->name
	if $ce->parent != 0
		printf " extends %s", $ce->parent->name
	end
	if $ce->num_interfaces != 0
		printf " implements"
		set $tmp = 0
		while $tmp < $ce->num_interfaces
			printf " %s", $ce->interfaces[$tmp]->name
			set $tmp = $tmp + 1
			if $tmp < $ce->num_interfaces
				printf ","
			end
		end
	end
	set $ce = $ce->parent
end

define ____print_inh_iface
	set $ce = $arg0
	printf "interface %s", $ce->name
	if $ce->num_interfaces != 0
		set $ce = $ce->interfaces[0]
		printf " extends %s", $ce->name
	else
		set $ce = 0
	end
end

define print_inh
	set $ce = $arg0
	set $depth = 0
	while $ce != 0
		set $tmp = $depth
		while $tmp != 0
			printf " "
			set $tmp = $tmp - 1
		end
		set $depth = $depth + 1
		if $ce->ce_flags & 0x80
			____print_inh_iface $ce
		else
			____print_inh_class $ce
		end
		printf " {\n"
	end
	while $depth != 0
		set $tmp = $depth
		while $tmp != 1
			printf " "
			set $tmp = $tmp - 1
		end
		printf "}\n"
		set $depth = $depth - 1
	end
end

define print_pi
	set $pi = $arg0
	printf "[%p] {\n", $pi
	printf "    h     = %lu\n", $pi->h
	printf "    flags = %d (", $pi->flags
	if $pi->flags & 0x100
		printf "ZEND_ACC_PUBLIC"
	else
		if $pi->flags & 0x200
			printf "ZEND_ACC_PROTECTED"
		else
			if $pi->flags & 0x400
				printf "ZEND_ACC_PRIVATE"
			else
				if $pi->flags & 0x800
					printf "ZEND_ACC_CHANGED"
				end
			end
		end
	end
	printf ")\n"
	printf "    name  = "
	____print_str $pi->name $pi->name_length
	printf "\n}\n"
end

define ____print_str
	set $tmp = 0
	set $str = $arg0
	printf "\""
	while $tmp < $arg1
		if $str[$tmp] > 32 && $str[$tmp] < 127
			printf "%c", $str[$tmp]
		else
			printf "\\%o", $str[$tmp]
		end
		set $tmp = $tmp + 1
	end
	printf "\""
end

define printzn
	____executor_globals
	set $ind = 0
	set $znode = $arg0
	if $znode->op_type == 1
		set $optype = "IS_CONST"
	end
	if $znode->op_type == 2 
		set $optype = "IS_TMP_VAR"
	end
	if $znode->op_type == 4 
		set $optype = "IS_VAR"
	end
	if $znode->op_type == 8
		set $optype = "IS_UNUSED"
	end

	printf "[%p] %s", $znode, $optype

	if $znode->op_type == 1
		printf ": "
		____printzv &$znode->u.constant 0
	end
	if $znode->op_type == 2
		printf ": "
		set $tvar = (union _temp_variable *)((char *)$eg.current_execute_data->Ts + $znode->u.var)
		____printzv ((union _temp_variable *)$tvar)->tmp_var 0
	end
	if $znode->op_type == 4
		printf ": "
		set $tvar = (union _temp_variable *)((char *)$eg.current_execute_data->Ts + $znode->u.var)
		____printzv *$tvar->var.ptr_ptr 0
	end
	if $znode->op_type == 8
		printf "\n"
	end
end

document printzn
	print type and content of znode.
	usage: printzn &opline->op1 
end

define printzops
	printf "op1 => " 
	printzn &execute_data->opline.op1
	printf "op2 => "
	printzn &execute_data->opline.op2
	printf "result => "
	printzn &execute_data->opline.result
end

document printzops
	dump operands of the current opline
end

define zbacktrace
	____executor_globals
	dump_bt $eg.current_execute_data
end

document zbacktrace
	prints backtrace.
	This command is almost a short cut for
	> (gdb) ____executor_globals
	> (gdb) dump_bt $eg.current_execute_data
end

define zmemcheck
	set $p = alloc_globals.head
	set $stat = "?"
	set $total_size = 0
	if $arg0 != 0
		set $not_found = 1
	else
		set $not_found = 0
	end
	printf " block      size      status file:line\n"
	printf "-------------------------------------------------------------------------------\n"
	while $p
		set $aptr = $p + sizeof(struct _zend_mem_header) + sizeof(align_test)
		if $arg0 == 0 || (void *)$aptr == (void *)$arg0
			if $p->magic == 0x7312f8dc 
				set $stat = "OK"
			end
			if $p->magic == 0x99954317
				set $stat = "FREED"
			end
			if $p->magic == 0xfb8277dc
				set $stat = "CACHED"
			end
			set $filename = strrchr($p->filename, '/')
			if !$filename
				set $filename = $p->filename
			else
				set $filename = $filename + 1
			end
			printf " %p ", $aptr
			if $p->size == sizeof(struct _zval_struct) && ((struct _zval_struct *)$aptr)->type >= 0 && ((struct _zval_struct *)$aptr)->type < 10
				printf "ZVAL?(%-2d) ", $p->size
			else
				printf "%-9d ", $p->size
			end
			set $total_size = $total_size + $p->size
			printf "%-06s %s:%d", $stat, $filename, $p->lineno
			if $p->orig_filename
				set $orig_filename = strrchr($p->orig_filename, '/')
				if !$orig_filename
					set $orig_filename = $p->orig_filename
				else
					set $orig_filename = $orig_filename + 1
				end
				printf " <= %s:%d\n", $orig_filename, $p->orig_lineno
			else
				printf "\n"
			end
			if $arg0 != 0
				set $p = 0
				set $not_found = 0
			else
				set $p = $p->pNext
			end
		else
			set $p = $p->pNext
		end
	end
	if $not_found
		printf "no such block that begins at %p.\n", $aptr
	end
	if $arg0 == 0
		printf "-------------------------------------------------------------------------------\n"
		printf "     total: %d bytes\n", $total_size
	end
end

document zmemcheck
	show status of a memory block.
	usage: zmemcheck [ptr].
	if ptr is 0, all blocks will be listed.
end
