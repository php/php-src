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
	print type and content of znode
end

