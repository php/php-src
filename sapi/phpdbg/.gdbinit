define ____phpdbg_globals
	if basic_functions_module.zts
		if !$tsrm_ls
			set $tsrm_ls = ts_resource_ex(0, 0)
		end
		set $phpdbg = ((zend_phpdbg_globals*) (*((void ***) $tsrm_ls))[phpdbg_globals_id-1])
	else
		set $phpdbg = phpdbg_globals
	end
end
