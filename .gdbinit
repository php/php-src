define dump_bt
    set $t = $arg0
    while $t
		printf "[0x%08x] ", $t
		if $t->function_state.function->common.function_name
			printf "%s() ", $t->function_state.function->common.function_name
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
	set $ind = 0
	____printzv $arg0 0 
end

document printzv
	prints content of zval 
end

define ____printzv
	set $zvalue = $arg0

	if $zvalue->type == 0
		set $typename = "NULL"
	end
	if $zvalue->type == 1
		set $typename = "long"
	end
	if $zvalue->type == 2
		set $typename = "double"
	end
	if $zvalue->type == 3
		set $typename = "string"
	end
	if $zvalue->type == 4
		set $typename = "array"
	end
	if $zvalue->type == 5
		set $typename = "object"
	end
	if $zvalue->type == 6
		set $typename = "bool"
	end
	if $zvalue->type == 7
		set $typename = "resource"
	end
	if $zvalue->type == 8 
		set $typename = "constant"
	end
	if $zvalue->type == 9
		set $typename = "const_array"
	end

	printf "[0x%08x] ", $zvalue

	if $zvalue == executor_globals.uninitialized_zval_ptr
		printf "*uninitialized* "
	end
	printf "(refcount=%d) %s: ", $zvalue->refcount, $typename
	if $zvalue->type == 1
		printf "%ld", $zvalue->value.lval
	end
	if $zvalue->type == 2
		printf "%lf", $zvalue->value.dval
	end
	if $zvalue->type == 3
		printf "\"%s\"(%d)", $zvalue->value.str.val, $zvalue->value.str.len
	end
	if $zvalue->type == 4
		if ! $arg1
			printf "{\n"
			set $ind = $ind + 1
			____print_ht $zvalue->value.ht
			set $ind = $ind - 1
			set $i = $ind
			while $i > 0
				printf "  "
				set $i = $i - 1
			end
			printf "}"
		end
	end
	if $zvalue->type == 5
		if ! $arg1
			printf "(prop examination disabled due to a gdb bug)"
#			set $ht = $zvalue->value.obj.handlers->get_properties($zvalue)
#			printf "{\n"
#			set $ind = $ind + 1
#			____print_ht $ht
#			set $ind = $ind - 1
#			set $i = $ind
#			while $i > 0
#				printf "  "
#				set $i = $i - 1
#			end
#			printf "}"
		end
	end
	if $zvalue->type == 6
		if $zvalue->lval
			printf "true"
		else
			printf "false"
		end
	end
	if $zvalue->type == 7
		printf "#%d", $zvalue->value.lval
	end
	printf "\n"
end

define ____print_ht
	set $ht = $arg0
	set $p = $ht->pListHead

	while $p != 0
		set $zval = *(struct _zval_struct **)$p->pData

		set $i = $ind
		while $i > 0
			printf "  "
			set $i = $i - 1
		end

		if $p->nKeyLength > 0 
			printf "\"%s\" => ", $p->arKey
		else
			printf "%d => ", $p->h
		end

		____printzv $zval 1
		set $p = $p->pListNext
	end
end

define print_ht
	set $ind = 1
	printf "[0x%08x] {\n", $arg0
	____print_ht $arg0
	printf "}\n"
end

document print_ht
	dumps elements of HashTable made of zval
end

define printzn
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

	printf "[0x%08x] %s", $znode, $optype

	if $znode->op_type == 1
		printf ": "
		____printzv &$znode->u.constant
	end
	if $znode->op_type == 2
		printf ": "
		set $tvar = (union _temp_variable *)((char *)executor_globals.current_execute_data->Ts + $znode->u.var)
		____printzv ((union _temp_variable *)$tvar)->tmp_var
	end
	if $znode->op_type == 4
		printf ": "
		set $tvar = (union _temp_variable *)((char *)executor_globals.current_execute_data->Ts + $znode->u.var)
		____printzv *$tvar->var.ptr_ptr
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
			printf " 0x%08x ", $aptr
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
		printf "no such block that begins at 0x%08x.\n", $aptr 
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

