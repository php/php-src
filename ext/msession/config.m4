
PHP_ARG_WITH(msession, for msession support,
        [  --with-msession             Include msession support])
 
if test "$PHP_MSESSION" != "no"; then
	PHOENIX_HOME="/local/projects/phoenix"
  	AC_DEFINE(HAVE_MSESSION, 1, [ ])
  	PHP_EXTENSION(msession, $ext_shared)
	PHOENIX_INCLUDE_="-I/local/projects/phoenix/lib"
  	PHP_ADD_LIBRARY_WITH_PATH(phoenix, "/local/projects/phoenix/lib", PHOENIX_LIB)

	PHP_SUBST(PHOENIX_INCLUDE)
	PHP_SUBST(PHOENIX_LIB)
fi

