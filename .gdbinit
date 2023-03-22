# Load this file: '$ gdb -ix $PHPREPO/.gdbinit ...'
# Functions starting with four underscores like "____foo" are used as helper
# functions.

define set_ts
	set $tsrm_ls = $arg0
end

# TODO: add Usage
document set_ts
	set the ts resource, it is impossible for gdb to
	call ts_resource_ex while no process is running,
	but we could get the resource from the argument
	of frame info.
end

define ____executor_globals
	if basic_functions_module.zts
		set $tsrm_ls = _tsrm_ls_cache
		set $eg = ((zend_executor_globals*) (*((void ***) $tsrm_ls))[executor_globals_id-1])
		set $cg = ((zend_compiler_globals*) (*((void ***) $tsrm_ls))[compiler_globals_id-1])
		set $eg_ptr = $eg
	else
		set $eg = executor_globals
		set $cg = compiler_globals
		set $eg_ptr = (zend_executor_globals*) &executor_globals
	end
end

document ____executor_globals
	portable way of accessing executor_globals, set as $eg
	this also sets compiler_globals to $cg
	ZTS detection is automatically based on ext/standard module struct
end

# print compiled variables and their values
# $arg0: zend_execute_data*
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
	If a zend_execute_data pointer is set, this will print the compiled
	variables of that scope. If no parameter is used, it will use
	current_execute_data for scope.

	Usage: print_cvs [(zend_execute_data*)ped]
end

# arg0: zend_execute_data*
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
	dumps execution stack of PHP code.

	Usage: dump_bt (zend_execute_data*)ped
	Example: dump_bt executor_globals.current_execute_data
	See also: zbacktrace
end

# $arg0: zval*
define printzv
	set $ind = 1
	____printzv $arg0 0
end

document printzv
	prints zval address and contents
	Usage: printzv (zval*)pzval
end

# $arg0: zval*
# $arg1: used for array, object, reference, indirect type
#        0: print extra information
#        1: NOT print extra information
define ____printzv_contents
	set $zvalue = $arg0

	# Types are defined in zend_type_code.h
	set $type = $zvalue->u1.v.type

	# For refcounted type, print refcount
	# IS_STRING = 6 &&  IS_CONSTANT_AST = 11
	# STRING, ARRAY, OBJECT, RESOURCE, REFERENCE, CONSTANT_AST
	if $type > 5 && $type < 12
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
		# if $arg1 == 0, then print array values
		if ! $arg1
			set $ind = $ind + 1
			# 1: HashTable made of zval (zval*)
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
		# if $arg1 == 0, then print property information
		if ! $arg1
			if $handlers->get_properties == &zend_std_get_properties
				if $zobj->properties
					printf "\nProperties "
					set $ht = $zobj->properties
					set $ind = $ind + 1
					____print_ht $ht 1
					set $ind = $ind - 1
					set $i = $ind
					while $i > 0
						printf "  "
						set $i = $i - 1
					end
				else
					printf " {\n"
					set $ht = &$zobj->ce->properties_info
					set $k = 0
					set $num = $ht->nNumUsed
					while $k < $num
						set $p = (Bucket*)($ht->arData + $k)
						set $name = $p->key
						set $prop = (zend_property_info*)$p->val.value.ptr
						set $val = (zval*)((char*)$zobj + $prop->offset)
						printf "%s => ", $name->val
						printzv $val
						set $k = $k + 1
					end
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
		printf "CONSTANT_AST"
	end

	# internal types
	if $type == 12
		printf "indirect: "
		____printzv $zvalue->value.zv $arg1
	end
	if $type == 13
		printf "pointer: %p", $zvalue->value.ptr
	end
	if $type == 15
		printf "_ERROR"
	end
	# 	_IS_BOOL = 18
	if $type == 18
		printf "_BOOL"
	end
	# _IS_NUMBER = 19
	if $type == 19
		printf "_NUMBER"
	end
	if $type > 19
		printf "unknown type %d", $type
	end
	printf "\n"
end

# $arg0: zval*
# $arg1: 0: print extra information; 1: not
#        Used only for array, object, reference, indirect type
define ____printzv
	____executor_globals
	set $zvalue = $arg0

	printf "[%p] ", $zvalue

	set $zcontents = (zval*) $zvalue
	____printzv_contents $zcontents $arg1
	# if $arg1
	# 	____printzv_contents $zcontents $arg1
	# else
	# 	____printzv_contents $zcontents 0
	# end
end

define print_global_vars
	____executor_globals
	set $symtable = ((HashTable *)&($eg_ptr->symbol_table))
	print_ht $symtable
end

document print_global_vars
	Prints the global variables
	Usage: print_global_vars
end

define print_const_table
	set $ind = 1
	printf "[%p] {\n", $arg0
	____print_ht $arg0 4
	printf "}\n"
end

document print_const_table
	Dumps elements of Constants HashTable
	Usage: print_const_table (HashTable*)pct
	Example: print_const_table executor_globals.zend_constants
end

# arg0: (HashTable*)
# arg1: Hash value type
# 		0: HashTable made of pointers (void*)
# 		1: HashTable made of zval (zval*)
# 		2: HashTable made of strings (char*)
# 		3: HashTable made of zend_function*
# 		4: HashTable made of zend_constant*
define ____print_ht
	set $ht = (HashTable*)$arg0

	# $ind is set before calling this function
	set $n = $ind
	while $n > 0
		printf "  "
		set $n = $n - 1
	end

	# HASH_FLAG_PACKED    (1<<2)
	set $packed = $ht->u.v.flags & 4
	if $packed
		printf "Packed"
	else
		printf "Hash"
	end
	printf "(%d)[%p] ", $ht->nNumOfElements, $ht
	printf "flags=[0x%x] ", $ht->u.v.flags
	printf "pDestructor=["
	output $ht->pDestructor
	printf "]\n{\n"

	set $num = $ht->nNumUsed
	set $i = 0
	set $ind = $ind + 1
	while $i < $num
		if $packed
			set $val = (zval*)($ht->arPacked + $i)
			set $key = (zend_string*)0
			set $h = $i
		else
			set $bucket = (Bucket*)($ht->arData + $i)
			set $val = &($bucket->val)
			set $key = $bucket->key
			set $h = $bucket->h
		end

		set $n = $ind
		if $val->u1.v.type > 0
			while $n > 0
				printf "  "
				set $n = $n - 1
			end
			printf "[%d] ", $i

			# print hash key
			if $key
				# for bucket, $key != 0 and is zend_string*;
				____print_str $key->val $key->len
				printf " => "
			else
				# for packed $key == 0
				printf "%d => ", $h
			end

			# print hash value
			if $arg1 == 0
				printf "%p\n", $val
			end
			if $arg1 == 1
				# print_ht goes here
				set $zval = $val
				____printzv $zval 1
			end
			if $arg1 == 2
				printf "%s\n", (char*)$val->value.ptr
			end
			if $arg1 == 3
				set $func = (zend_function*)$val->value.ptr
				printf "\"%s\"\n", $func->common.function_name->val
			end
			if $arg1 == 4
				set $const = (zend_constant *)$val->value.ptr
				____printzv $const 1
			end
		end
		set $i = $i + 1
	end
	set $ind = $ind - 1
	printf "}\n"
end

# arg0: (HashTable*)
define print_ht
	set $ind = 0
	____print_ht $arg0 1
end

document print_ht
	dumps elements of HashTable made of zval
	Usage: print_ht (HashTable*)pht
end

define print_htptr
	set $ind = 0
	____print_ht $arg0 0
end

document print_htptr
	dumps elements of HashTable made of pointers
	Usage: print_ht (HashTable*)pht
end

define print_htstr
	set $ind = 0
	____print_ht $arg0 2
end

document print_htstr
	dumps elements of HashTable made of strings
	Usage: print_ht (HashTable*)pht
end

define print_ft
	set $ind = 0
	____print_ht $arg0 3
end

document print_ft
	dumps a function table (HashTable)
	Usage: print_ft (HashTable*)pft
end

# print inheritance class information
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

# print inheritance interface information
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

# print zend inheritance information
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

# TODO: add more information
document print_inh
	Usage: print_inh (zend_class_entry*)pce
end

define print_pi
	set $pi = (zend_property_info *)$arg0
	set $initial_offset = ((uint32_t)(uintptr_t)(&((zend_object*)0)->properties_table[(0)]))
	set $ptr_to_val = (zval*)((char*)$pi->ce->default_properties_table + $pi->offset - $initial_offset)
	printf "[%p] {\n", $pi
	printf "    offset = %p\n", $pi->offset
	printf "    ce = [%p] %s\n", $pi->ce, $pi->ce->name->val
	printf "    flags = 0x%x (", $pi->flags
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
					printf "ZEND_ACC_EARLY_BINDING"
				else
					if $pi->flags & 0x20000
						printf "ZEND_ACC_SHADOW"
					end
				end
			end
		end
	end
	printf ")\n"
	printf "    name  = "
	print_zstr $pi->name
	printf "    default value: "
	printzv $ptr_to_val
	printf "}\n"
end

document print_pi
	Takes a pointer to an object's property and prints the property information
	Usage: print_pi <ptr>
end

# arg0: char*, string to print
# arg1: int, length of the string
# arg2: int, maxlen to print
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
		# print letters as is
		# print special characters in octal (base 8)
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

# $arg0: znode*
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

	# IS_CONST
	if $znode->op_type == 1
		printf ": "
		____printzv &$znode->u.constant 0
	end

	# IS_TMP_VAR
	if $znode->op_type == 2
		printf ": "
		set $tvar = (union _temp_variable *)((char *)$eg.current_execute_data->Ts + $znode->u.var)
		____printzv ((union _temp_variable *)$tvar)->tmp_var 0
	end

	# IS_VAR
	if $znode->op_type == 4
		printf ": "
		set $tvar = (union _temp_variable *)((char *)$eg.current_execute_data->Ts + $znode->u.var)
		____printzv *$tvar->var.ptr_ptr 0
	end

	# IS_UNUSED
	if $znode->op_type == 8
		printf "\n"
	end
end

document printzn
	print type and content of znode.
	Usage: printzn (znode*)pznode
end

# print znode_op information
# $arg0: znode_op*
define ____print_znode_op
	set $op = $arg0
	printf "[%p]\n", $op
	printf "  constant=[%d] ", $op->constant
	printf "var=[%d] ", $op->var
	printf "num=[%d] ", $op->num
	printf "opline_num=[%d] ", $op->opline_num
	printf "jmp_offset=[%d]\n", $op->jmp_offset
end

document ____print_znode_op
	Dump znode_op information
	Usage: ____print_znode_op (znode_op*)pop
end

# op1, op2, and result are actually of znode_op type
define printzops
	printf "op1 => "
	____print_znode_op &execute_data->opline.op1
	printf "op2 => "
	____print_znode_op &execute_data->opline.op2
	printf "result => "
	____print_znode_op &execute_data->opline.result
end

document printzops
	Dump operands of the current opline
	Usage: printzops
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
	Print the length and contents of a zend string
	Usage: print_zstr (zend_string*)ptr [max length]
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
	Usage: lookup_root [ptr].
end

#### Functions to dump opline information ####
# TODO: print more meaningful information
# arg0: (zend_op*)
define dump_opline
	if $arg0
		set $opline=$arg0
		printf "lineno=[%d] opcode=[%d] ", $opline->lineno, $opline->opcode
		printf "handler=["
		output $opline->handler
		printf "]\n"
	else
		printf "[ERROR] Invalid zend_op* argument is NULL\n"
	end

end

document dump_opline
	Print opline information
	Usage: dump_opline (zend_op*)opline
end

define dump_current_opline
	____executor_globals
	if $eg.current_execute_data
		dump_opline $eg.current_execute_data->opline
	else
		printf "[ERROR] $eg.current_execute_data is NULL\n"
	end
end

document dump_current_opline
	Dump currently executed opline information
	Usage: print_executed_opline
end

# arg0: (zend_op_array*)
define dump_op_array
	set $ops = (zend_op_array*) $arg0

	# print file name and line#
	printf "filename = < %s:", $ops->filename.val
	printf "%d-%d >\n", $ops->line_start, $ops->line_end

	# print function name if has
	if $ops->function_name
		set $funcname = $ops->function_name.val
	else
		set $funcname = "Empty"
	end
	printf "func=<%s> ", $funcname

	printf "type=[%d]\n", $ops->type

	# dump each opline
	set $i = 0
	while $i < $ops->last
		set $opline = $ops->opcodes + $i
		printf "index=[%d] ", $i
		dump_opline $opline
		set $i = $i + 1
	end
end

document dump_op_array
	Dump opline information in an op_array
	Usage: dump_op_array (zend_op_array*)poparray
end

define dump_current_op_array
	____executor_globals
	dump_op_array &($eg.current_execute_data->func->op_array)
end

document dump_current_op_array
	Dump opline information of current function
	Usage: dump_current_op_array
end

#### functions to set breakpoint ####
# set breakpoint at opline->handler
# $arg0: (zend_op*)opline
define bp_opline
	set $opline = $arg0
	eval "break *%p", $opline->handler
end

document bp_opline
	Set breakpoint at opline->handler
	Usage: bp_opline (zend_op*)opline
end

# $arg0: (zend_op_array*)
# set breakpoint at every opline->handler in the op_array
define bp_op_array
	set $ops = (zend_op_array*) $arg0

	set $i = 0
	while $i < $ops->last
		set $opline = $ops->opcodes + $i
		bp_opline $opline
		set $i = $i + 1
	end
end

document bp_op_array
	Set breakpoint at each opline handler in op_array
	Usage: bp_op_array (zend_op_array*)poparray
end

define bp_current_op_array
	____executor_globals
	bp_op_array_handler &($eg.current_execute_data->func->op_array)
end

document bp_current_op_array
	Set breakpoint at each opline handler in current op_array
	Usage: bp_current_op_array
end
