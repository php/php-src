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
	if $argc == 0
		____executor_globals
		set $cv_ex_ptr = $eg.current_execute_data
	else
		set $cv_ex_ptr = (zend_execute_data *)$arg0
	end
	set $cv_count = $cv_ex_ptr.func.op_array.last_var
	set $cv = $cv_ex_ptr.func.op_array.vars
	set $cv_idx = 0
	set $callFrameSize = (sizeof(zend_execute_data) + sizeof(zval) - 1) / sizeof(zval)

	printf "Compiled variables count: %d\n\n", $cv_count
	while $cv_idx < $cv_count
		printf "[%d] '%s'\n", $cv_idx, $cv[$cv_idx].val
		set $zvalue = ((zval *) $cv_ex_ptr) + $callFrameSize + $cv_idx
		printzv $zvalue
		set $cv_idx = $cv_idx + 1
	end
end

document print_cvs
	Prints the compiled variables and their values.
	If a zend_execute_data pointer is set this will print the compiled
	variables of that scope. If no parameter is used it will use
	current_execute_data for scope.

	usage: print_cvs [zend_execute_data *]
end

define dump_bt
	set $ex = $arg0
	while $ex
		printf "[%p] ", $ex
		set $func = $ex->func
		if $func
			if $ex->This->value.obj
				if $func->common.scope
					printf "%s->", $func->common.scope->name->val
				else
					printf "%s->", $ex->This->value.obj->ce.name->val
				end
			else
				if $func->common.scope
					printf "%s::", $func->common.scope->name->val
				end
			end

			if $func->common.function_name
				printf "%s(", $func->common.function_name->val
			else
				printf "(main"
			end

			set $callFrameSize = (sizeof(zend_execute_data) + sizeof(zval) - 1) / sizeof(zval)

			set $count = $ex->This.u2.num_args
			set $arg = 0
			while $arg < $count
				if $arg > 0
					printf ", "
				end

				set $zvalue = (zval *) $ex + $callFrameSize + $arg
				set $type = $zvalue->u1.v.type
				if $type == 1
					printf "NULL"
				end
				if $type == 2
					printf "false"
				end
				if $type == 3
					printf "true"
				end
				if $type == 4
					printf "%ld", $zvalue->value.lval
				end
				if $type == 5
					printf "%f", $zvalue->value.dval
				end
				if $type == 6
					____print_str $zvalue->value.str->val $zvalue->value.str->len
				end
				if $type == 7
					printf "array(%d)[%p]", $zvalue->value.arr->nNumOfElements, $zvalue
				end
				if $type == 8
					printf "object[%p]", $zvalue
				end
				if $type == 9
					printf "resource(#%d)", $zvalue->value.lval
				end
				if $type == 10
					printf "reference"
				end
				if $type > 10
					printf "unknown type %d", $type
				end
				set $arg = $arg + 1
			end

			printf ") "
		else
			printf "??? "
		end
		if $func != 0
			if $func->type == 2
				printf "%s:%d ", $func->op_array.filename->val, $ex->opline->lineno
			else
				printf "[internal function]"
			end
		end
		set $ex = $ex->prev_execute_data
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
	set $type = $zvalue->u1.v.type

	# 15 == IS_INDIRECT
	if $type > 5 && $type != 15
		printf "(refcount=%d) ", $zvalue->value.counted->gc.refcount
	end

	if $type == 0
		printf "UNDEF"
	end
	if $type == 1
        printf "NULL"
	end
	if $type == 2
		printf "bool: false"
	end
	if $type == 3
		printf "bool: true"
	end
    if $type == 4
		printf "long: %ld", $zvalue->value.lval
    end
    if $type == 5
        printf "double: %f", $zvalue->value.dval
    end
    if $type == 6
       printf "string: %s", $zvalue->value.str->val
    end
	if $type == 7 
		printf "array: "
		if ! $arg1
			set $ind = $ind + 1
			____print_ht $zvalue->value.arr 1
			set $ind = $ind - 1
			set $i = $ind
			while $i > 0
				printf "  "
				set $i = $i - 1
			end
		end
		set $type = 0
	end
	if $type == 8
		printf "object"
		____executor_globals
		set $handle = $zvalue->value.obj.handle
		set $handlers = $zvalue->value.obj.handlers
		set $zobj = $zvalue->value.obj
		set $cname = $zobj->ce->name->val
		printf "(%s) #%d", $cname, $handle
		if ! $arg1
			if $handlers->get_properties == &zend_std_get_properties
				if $zobj->properties
					set $ht = $zobj->properties
				else
					set $ht = &$zobj->ce->properties_info
				end
				printf "\nProperties "
				if $ht
					set $ind = $ind + 1
					____print_ht $ht 1
					set $ind = $ind - 1
					set $i = $ind
					while $i > 0
						printf "  "
						set $i = $i - 1
					end
				else
					echo "not found"
				end
			end
		end
		set $type = 0
	end
	if $type == 9
		printf "resource: #%d", $zvalue->value.res->handle
	end
	if $type == 10
		printf "reference: "
		____printzv &$zvalue->value.ref->val $arg1
	end
	if $type == 11
		printf "const: %s", $zvalue->value.str->val
	end
	if $type == 12
		printf "const_ast"
	end
	if $type == 13
		printf "_IS_BOOL"
	end
	if $type == 14
		printf "IS_CALLABLE"
	end
	if $type == 15
		printf "indirect: "
		____printzv $zvalue->value.zv $arg1
	end
	if $type == 16
		printf "string_offset"
	end
	if $type == 17
		printf "pointer: %p", $zvalue->value.ptr
	end
	if $type > 17
		printf "unknown type %d", $type
	end
	printf "\n"
end

define ____printzv
	____executor_globals
	set $zvalue = $arg0

	printf "[%p] ", $zvalue

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
	set $n = $ind
	while $n > 0
		printf "  "
		set $n = $n - 1
	end

	if $ht->u.v.flags & 4
		printf "Packed"
	else
		printf "Hash"
	end
	printf "(%d)[%p]: {\n", $ht->nNumOfElements, $ht

	set $num = $ht->nNumUsed
	set $i = 0
	set $ind = $ind + 1
	while $i < $num
		set $p = (Bucket*)($ht->arData + $i)
		set $n = $ind
		if $p->val.u1.v.type > 0
			while $n > 0
				printf "  "
				set $n = $n - 1
			end
			printf "[%d] ", $i
			if $p->key 
				printf "%s => ", $p->key->val
			else
				printf "%d => ", $p->h
			end
			if $arg1 == 0
				printf "%p\n", (zval *)&$p->val
			end
			if $arg1 == 1
				set $zval = (zval *)&$p->val
				____printzv $zval 1
			end
			if $arg1 == 2
				printf "%s\n", (char*)$p->val.value.ptr
			end
			if $arg1 == 3
				set $func = (zend_function*)$p->val.value.ptr
				printf "\"%s\"\n", $func->common.function_name->val
			end
		end
		set $i = $i + 1
	end
	set $ind = $ind - 1
	printf "}\n"
end

define print_ht
	set $ind = 0
	____print_ht $arg0 1
end

document print_ht
	dumps elements of HashTable made of zval
end

define print_htptr
	set $ind = 0
	____print_ht $arg0 0
end

document print_htptr
	dumps elements of HashTable made of pointers
end

define print_htstr
	set $ind = 0 
	____print_ht $arg0 2
end

document print_htstr
	dumps elements of HashTable made of strings
end

define print_ft
	set $ind = 0
	____print_ht $arg0 3
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
	printf "class %s", $ce->name->val
	if $ce->parent != 0
		printf " extends %s", $ce->parent->name->val
	end
	if $ce->num_interfaces != 0
		printf " implements"
		set $tmp = 0
		while $tmp < $ce->num_interfaces
			printf " %s", $ce->interfaces[$tmp]->name->val
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
	printf "interface %s", $ce->name->val
	if $ce->num_interfaces != 0
		set $ce = $ce->interfaces[0]
		printf " extends %s", $ce->name->val
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
	set $pi = (zend_property_info *)$arg0
	printf "[%p] {\n", $pi
	printf "    ce = [%p] %s\n", $pi->ce, $pi->ce->name->val
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
	print_zstr $pi->name
	printf "}\n"
end

document print_pi
	Takes a pointer to an object's property and prints the property information
	usage: print_pi <ptr>
end

define ____print_str
	set $tmp = 0
	set $str = $arg0
	if $argc > 2
		set $maxlen = $arg2
	else
		set $maxlen = 256
	end

	printf "\""
	while $tmp < $arg1 && $tmp < $maxlen
		if $str[$tmp] > 31 && $str[$tmp] < 127
			printf "%c", $str[$tmp]
		else
			printf "\\%o", $str[$tmp]
		end
		set $tmp = $tmp + 1
	end
	if $tmp != $arg1
		printf "..."
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

define print_zstr
	set $zstr = (zend_string *)$arg0
	if $argc == 2
		set $maxlen = $arg1
	else
		set $maxlen = $zstr->len
	end
	printf "string(%d) ", $zstr->len
	____print_str $zstr->val $zstr->len $maxlen
	printf "\n"
end

document print_zstr
	print the length and contents of a zend string
	usage: print_zstr <ptr> [max length]
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

define lookup_root
	set $found = 0
	if gc_globals->roots
		set $current = gc_globals->roots->next
		printf "looking ref %p in roots\n", $arg0
		while $current != &gc_globals->roots
			if $current->ref == $arg0
				set $found = $current
				break
			end
			set $current = $current->next
		end
		if $found != 0
			printf "found root %p\n", $found
		else
			printf "not found\n"
		end
	end
end

document lookup_root
	lookup a refcounted in root
	usage: lookup_root [ptr].
end
