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
