/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_SWF
#include <stdio.h>
#include <swf.h>
#include "ext/standard/info.h"
#include "php_open_temporary_file.h"
#include "php_swf.h"

ZEND_DECLARE_MODULE_GLOBALS(swf)

/* {{{ swf_functions[]
 */
function_entry swf_functions[] = {
	PHP_FE(swf_openfile,		NULL)
	PHP_FE(swf_closefile,		NULL)
	PHP_FE(swf_labelframe,		NULL)
	PHP_FE(swf_showframe,		NULL)
	PHP_FE(swf_setframe,		NULL)
	PHP_FE(swf_getframe,		NULL)
	PHP_FE(swf_mulcolor,		NULL)
	PHP_FE(swf_addcolor,		NULL)
	PHP_FE(swf_placeobject,		NULL)
	PHP_FE(swf_modifyobject,		NULL)
	PHP_FE(swf_removeobject,		NULL)
	PHP_FE(swf_nextid,		NULL)
	PHP_FE(swf_startdoaction,		NULL)
	PHP_FE(swf_enddoaction,		NULL)
	PHP_FE(swf_actiongotoframe,		NULL)
	PHP_FE(swf_actiongeturl,		NULL)
	PHP_FE(swf_actionnextframe,		NULL)
	PHP_FE(swf_actionprevframe,		NULL)
	PHP_FE(swf_actionplay,		NULL)
	PHP_FE(swf_actionstop,		NULL)
	PHP_FE(swf_actiontogglequality,		NULL)
	PHP_FE(swf_actionwaitforframe,		NULL)
	PHP_FE(swf_actionsettarget,		NULL)
	PHP_FE(swf_actiongotolabel,		NULL)
	PHP_FE(swf_defineline,		NULL)
	PHP_FE(swf_definerect,		NULL)
	PHP_FE(swf_definepoly,		NULL)
	PHP_FE(swf_startshape,		NULL)
	PHP_FE(swf_shapelinesolid,		NULL)
	PHP_FE(swf_shapefilloff,		NULL)
	PHP_FE(swf_shapefillsolid,		NULL)
	PHP_FE(swf_shapefillbitmapclip,		NULL)
	PHP_FE(swf_shapefillbitmaptile,		NULL)
	PHP_FE(swf_shapemoveto,		NULL)
	PHP_FE(swf_shapelineto,		NULL)
	PHP_FE(swf_shapecurveto,		NULL)
	PHP_FE(swf_shapecurveto3,		NULL)
	PHP_FE(swf_shapearc,		NULL)
	PHP_FE(swf_endshape,		NULL)
	PHP_FE(swf_definefont,		NULL)
	PHP_FE(swf_setfont,		NULL)
	PHP_FE(swf_fontsize,		NULL)
	PHP_FE(swf_fontslant,		NULL)
	PHP_FE(swf_fonttracking,		NULL)
	PHP_FE(swf_getfontinfo,		NULL)
	PHP_FE(swf_definetext,		NULL)
	PHP_FE(swf_textwidth,		NULL)
	PHP_FE(swf_definebitmap,		NULL)
	PHP_FE(swf_getbitmapinfo,		NULL)
	PHP_FE(swf_startsymbol,		NULL)
	PHP_FE(swf_endsymbol,		NULL)
	PHP_FE(swf_startbutton,		NULL)
	PHP_FE(swf_addbuttonrecord,		NULL)
	PHP_FE(swf_oncondition,		NULL)
	PHP_FE(swf_endbutton,		NULL)
	PHP_FE(swf_viewport,		NULL)
	PHP_FE(swf_ortho,		NULL)
	PHP_FE(swf_ortho2,		NULL)
	PHP_FE(swf_perspective,		NULL)
	PHP_FE(swf_polarview,		NULL)
	PHP_FE(swf_lookat,		NULL)
	PHP_FE(swf_pushmatrix,		NULL)
	PHP_FE(swf_popmatrix,		NULL)
	PHP_FE(swf_scale,		NULL)
	PHP_FE(swf_translate,		NULL)
	PHP_FE(swf_rotate,		NULL)
	PHP_FE(swf_posround,		NULL)
	{NULL,NULL,NULL}
};
/* }}} */

/* {{{ swf_module_entry
 */
zend_module_entry swf_module_entry = {
    STANDARD_MODULE_HEADER,
	"swf",
	swf_functions,
	PHP_MINIT(swf),
	NULL,
	PHP_RINIT(swf),
	NULL,
	PHP_MINFO(swf),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SWF
ZEND_GET_MODULE(swf)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(swf)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "swf support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ _swf_init_globals
 */
static void _swf_init_globals(zend_swf_globals *sg)
{
	memset(sg, 0, sizeof(zend_swf_globals));
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(swf)
{
	ZEND_INIT_MODULE_GLOBALS(swf, _swf_init_globals, NULL);

	REGISTER_LONG_CONSTANT("MOD_COLOR", MOD_COLOR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MOD_MATRIX", MOD_MATRIX, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPE_PUSHBUTTON", TYPE_PUSHBUTTON, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TYPE_MENUBUTTON", TYPE_MENUBUTTON, CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("BSHitTest", BSHitTest, CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("BSDown", BSDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("BSOver", BSOver, CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("BSUp", BSUp, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OverDowntoIdle", OverDowntoIdle, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IdletoOverDown", IdletoOverDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OutDowntoIdle", OutDowntoIdle, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OutDowntoOverDown", OutDowntoOverDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OverDowntoOutDown", OverDowntoOutDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OverUptoOverDown", OverUptoOverDown, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OverUptoIdle", OverUptoIdle, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IdletoOverUp", IdletoOverUp, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ButtonEnter", ButtonEnter, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ButtonExit", ButtonExit, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MenuEnter", MenuEnter, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MenuExit", MenuExit, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(swf)
{
	SWFG(use_file) = 0;
	return SUCCESS;
}
/* }}} */
         
/* {{{ proto void swf_openfile(string name, float xsize, float ysize, float framerate, float r, float g, float b)
   Create a Shockwave Flash file given by name, with width xsize and height ysize at a frame rate of framerate and a background color specified by a red value of r, green value of g and a blue value of b */
PHP_FUNCTION(swf_openfile)
{
	zval **name, **sizeX, **sizeY, **frameRate, **r, **g, **b;
	char *na, *tmpna;
	zend_bool free_na = 0;
	
	if (ZEND_NUM_ARGS() != 7 ||
	    zend_get_parameters_ex(7, &name, &sizeX, &sizeY, &frameRate, &r, &g, &b) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(name);
	convert_to_double_ex(sizeX);
	convert_to_double_ex(sizeY);
	convert_to_double_ex(frameRate);
	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);


	tmpna = Z_STRVAL_PP(name);

	if (strcasecmp("php://stdout", tmpna) == 0) {
		FILE *fp;

		fp = php_open_temporary_file(NULL, "php_swf_stdout", &na TSRMLS_CC);
		if (!fp) {
			free_na = 0;
			RETURN_FALSE;
		}
		VCWD_UNLINK((const char *)na);
		fclose(fp);
		free_na = 1;
		SWFG(use_file) = 0;
	} else {
		na = tmpna;
		SWFG(use_file) = 1;
	}

#ifdef VIRTUAL_DIR
	if (virtual_filepath(na, &tmpna TSRMLS_CC)) {
		if (free_na) {
			efree(na);
		}
		return;
	}
	if (free_na) {
		efree(na);
	}
	na = tmpna;
#endif
	if (!SWFG(use_file))
		SWFG(tmpfile_name) = na;

	swf_openfile(na,(float)Z_DVAL_PP(sizeX), (float)Z_DVAL_PP(sizeY),
      		 	 (float)Z_DVAL_PP(frameRate), (float)Z_DVAL_PP(r), 
      		 	 (float)Z_DVAL_PP(g), (float)Z_DVAL_PP(b));
#ifdef VIRTUAL_DIR
	free(na);
#endif
}
/* }}} */

/* {{{ proto void swf_closefile(void)
   Close a Shockwave flash file that was opened with swf_openfile */
PHP_FUNCTION(swf_closefile)
{
	swf_closefile();
	
	if (!SWFG(use_file)) {
		FILE *f;
		char buf[4096];
		int b;
		
		if ((f = VCWD_FOPEN(SWFG(tmpfile_name), "r")) == NULL) {
			php_error(E_WARNING, "Cannot create temporary file for stdout support with SWF");
			RETURN_NULL();
		}
		
		while ((b = fread(buf, 1, sizeof(buf), f)) > 0)
			php_write(buf, b TSRMLS_CC);
		
		fclose(f);
		
		VCWD_UNLINK((const char *)SWFG(tmpfile_name));
		efree(SWFG(tmpfile_name));
	}
}
/* }}} */

/* {{{ proto void swf_labelframe(string name)
   Adds string name to the current frame */
PHP_FUNCTION(swf_labelframe)
{
	zval **name;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &name) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	swf_labelframe(Z_STRVAL_PP(name));
}
/* }}} */

/* {{{ proto void swf_showframe(void)
   Finish the current frame */
PHP_FUNCTION(swf_showframe)
{
	swf_showframe();
}
/* }}} */

/* {{{ proto void swf_setframe(int frame_number)
   Set the current frame number to the number given by frame_number */
PHP_FUNCTION(swf_setframe)
{
	zval **frameno;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &frameno) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(frameno);
	
	swf_setframe(Z_LVAL_PP(frameno));
}
/* }}} */

/* {{{ proto int swf_getframe(void)
   Returns the current frame */
PHP_FUNCTION(swf_getframe)
{
	RETURN_LONG(swf_getframe());
}
/* }}} */

/* {{{ col_swf
 */
void col_swf(INTERNAL_FUNCTION_PARAMETERS, int opt) {
	zval **r, **g, **b, **a;
	if (ZEND_NUM_ARGS() != 4 ||
	    zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);
	convert_to_double_ex(a);
	if (opt) {
		swf_addcolor((float)Z_DVAL_PP(r), (float)Z_DVAL_PP(g), (float)Z_DVAL_PP(b), (float)Z_DVAL_PP(a));
	} else {
		swf_mulcolor((float)Z_DVAL_PP(r), (float)Z_DVAL_PP(g), (float)Z_DVAL_PP(b), (float)Z_DVAL_PP(a));
	}
}
/* }}} */

/* {{{ proto void swf_mulcolor(float r, float g, float b, float a)
   Sets the global multiply color to the rgba value specified */
PHP_FUNCTION(swf_mulcolor)
{
	col_swf(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */ 
         
/* {{{ proto void swf_addcolor(float r, float g, float b, float a)
   Set the global add color to the rgba value specified */
PHP_FUNCTION(swf_addcolor)
{
	col_swf(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */ 
         
/* {{{ proto void swf_placeobject(int objid, int depth)
   Places the object, objid, in the current frame at depth, depth */
PHP_FUNCTION(swf_placeobject)
{
	zval **objid, **depth;
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &objid, &depth) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	convert_to_long_ex(depth);
	swf_placeobject(Z_LVAL_PP(objid), Z_LVAL_PP(depth));
}
/* }}} */

/* {{{ proto void swf_modifyobject(int depth, int how)
   Updates the position and/or color of the object */
PHP_FUNCTION(swf_modifyobject)
{
	zval **depth, **how;
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &depth, &how) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(depth);
	convert_to_long_ex(how);
	
	swf_modifyobject(Z_LVAL_PP(depth), Z_LVAL_PP(how));
}
/* }}} */

/* {{{ proto void swf_removeobject(int depth)
   Removes the object at the specified depth */
PHP_FUNCTION(swf_removeobject)
{
	zval **depth;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &depth) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(depth);
	
	swf_removeobject(Z_LVAL_PP(depth));
}
/* }}} */

/* {{{ proto int swf_nextid(void)
   Returns a free objid */
PHP_FUNCTION(swf_nextid)
{
	RETURN_LONG(swf_nextid());
}
/* }}} */

/* {{{ proto void swf_startdoaction(void)
   Starts the description of an action list for the current frame */
PHP_FUNCTION(swf_startdoaction)
{
	swf_startdoaction();
}
/* }}} */

/* {{{ proto void swf_enddoaction(void)
   Ends the list of actions to perform for the current frame */
PHP_FUNCTION(swf_enddoaction)
{
	swf_enddoaction();
}
/* }}} */
 
/* {{{ proto void swf_actiongotoframe(int frame_number)
   Causes the Flash movie to display the specified frame, frame_number, and then stop. */
PHP_FUNCTION(swf_actiongotoframe)
{
	zval **frameno;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &frameno) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(frameno);

	swf_actionGotoFrame(Z_LVAL_PP(frameno));
}
/* }}} */

/* {{{ proto void swf_actiongeturl(string url, string target)
   Gets the specified url */
PHP_FUNCTION(swf_actiongeturl)
{
	zval **url, **target;
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &url, &target) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(url);
	convert_to_string_ex(target);
	
	swf_actionGetURL(Z_STRVAL_PP(url), Z_STRVAL_PP(target));
}
/* }}} */

/* {{{ proto void swf_actionnextframe(void)
   Goes foward one frame */
PHP_FUNCTION(swf_actionnextframe)
{
	swf_actionNextFrame();
}
/* }}} */

/* {{{ proto void swf_actionprevframe(void)
   Goes backward one frame */
PHP_FUNCTION(swf_actionprevframe)
{
	swf_actionPrevFrame();
}
/* }}} */

/* {{{ proto void swf_actionplay(void)
   Starts playing the Flash movie from the current frame */
PHP_FUNCTION(swf_actionplay)
{
	swf_actionPlay();
}
/* }}} */

/* {{{ proto void swf_actionstop(void)
   Stops playing the Flash movie at the current frame */
PHP_FUNCTION(swf_actionstop)
{
	swf_actionStop();
}
/* }}} */

/* {{{ proto void swf_actiontogglequality(void)
   Toggles between high and low quality */
PHP_FUNCTION(swf_actiontogglequality)
{
	swf_actionToggleQuality();
}
/* }}} */

/* {{{ proto void swf_actionwaitforframe(int frame, int skipcount)
   If the specified frame has not been loaded, skip the specified number of actions in the action list */
PHP_FUNCTION(swf_actionwaitforframe)
{
	zval **frame, **skipcount;
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &frame, &skipcount) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(frame);
	convert_to_long_ex(skipcount);
	swf_actionWaitForFrame(Z_LVAL_PP(frame), Z_LVAL_PP(skipcount));
}
/* }}} */

/* {{{ proto void swf_actionsettarget(string target)
   Sets the context for actions */
PHP_FUNCTION(swf_actionsettarget)
{
	zval **target;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &target) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(target);
	swf_actionSetTarget(Z_STRVAL_PP(target));
}
/* }}} */

/* {{{ proto void swf_actiongotolabel(string label)
   Causes the flash movie to display the frame with the given label and then stop */
PHP_FUNCTION(swf_actiongotolabel)
{
	zval **label;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &label) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(label);
	swf_actionGoToLabel(Z_STRVAL_PP(label));
}
/* }}} */

/* {{{ php_swf_define
 */
void php_swf_define(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval **objid, **x1, **y1, **x2, **y2, **width;
	if (ZEND_NUM_ARGS() != 6 ||
	    zend_get_parameters_ex(6, &objid, &x1, &y1, &x2, &y2, &width) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(objid);
	convert_to_double_ex(x1);
	convert_to_double_ex(y1);
	convert_to_double_ex(x2);
	convert_to_double_ex(y2);
	convert_to_double_ex(width);
	
	if (opt) {
		swf_defineline(Z_LVAL_PP(objid), (float)Z_DVAL_PP(x1), (float)Z_DVAL_PP(y1),
	 	               (float)Z_DVAL_PP(x2), (float)Z_DVAL_PP(y2), (float)Z_DVAL_PP(width));
	} else {
		swf_definerect(Z_LVAL_PP(objid), (float)Z_DVAL_PP(x1), (float)Z_DVAL_PP(y1),
	 	               (float)Z_DVAL_PP(x2), (float)Z_DVAL_PP(y2), (float)Z_DVAL_PP(width));
	}
}
/* }}} */

/* {{{ proto void swf_defineline(int objid, float x1, float y1, float x2, float y2, float width)
   Create a line with object id, objid, starting from x1, y1 and going to x2, y2 with width, width */
PHP_FUNCTION(swf_defineline)
{
	php_swf_define(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto void swf_definerect(int objid, float x1, float y1, float x2, float y2, float width)
   Create a rectangle with object id, objid, the upper lefthand coordinate is given by x1, y1 the bottom right coordinate is x2, y2 and with is the width of the line */
PHP_FUNCTION(swf_definerect)
{
	php_swf_define(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto void swf_definepoly(int obj_id, array coords, int npoints, float width)
   Define a Polygon from an array of x,y coordinates, coords. */
PHP_FUNCTION(swf_definepoly)
{
	zval **obj_id, **coordinates, **NumPoints, **width, **var;
	int npoints, i;
	float coords[256][2];
	
	if (ZEND_NUM_ARGS() != 4 ||
	    zend_get_parameters_ex(4, &obj_id, &coordinates, &NumPoints, &width) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(obj_id);
	convert_to_long_ex(NumPoints);
	convert_to_double_ex(width);
	
	if (Z_TYPE_PP(coordinates) != IS_ARRAY) {
		return;
		php_error(E_WARNING, "Wrong datatype of second argument to swf_definepoly");
	}
	
	npoints = Z_LVAL_PP(NumPoints);
	for (i = 0; i < npoints; i++)
	{
		if (zend_hash_index_find(Z_ARRVAL_PP(coordinates), (i * 2), (void **)&var) == SUCCESS) {
			SEPARATE_ZVAL(var);
			convert_to_double_ex(var);
			coords[i][0] = (float)Z_DVAL_PP(var);
		}
		
		if (zend_hash_index_find(Z_ARRVAL_PP(coordinates), (i * 2) + 1, (void **)&var) == SUCCESS) {
			SEPARATE_ZVAL(var);
			convert_to_double_ex(var);
			coords[i][1] = (float)Z_DVAL_PP(var);
		}
		
	}
	swf_definepoly(Z_LVAL_PP(obj_id), coords, npoints, (float)Z_DVAL_PP(width));
}
/* }}} */

/* {{{ proto void swf_startshape(int objid)
   Initialize a new shape with object id, objid */
PHP_FUNCTION(swf_startshape)
{
	zval **objid;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &objid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	swf_startshape(Z_LVAL_PP(objid));
}
/* }}} */

/* {{{ proto void swf_shapelinesolid(float r, float g, float b, float a, float width)
   Create a line with color defined by rgba, and a width of width */
PHP_FUNCTION(swf_shapelinesolid)
{
	zval **r, **g, **b, **a, **width;
	if (ZEND_NUM_ARGS() != 5 ||
	    zend_get_parameters_ex(5, &r, &g, &b, &a, &width) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);
	convert_to_double_ex(a);
	convert_to_double_ex(width);
	swf_shapelinesolid((float)Z_DVAL_PP(r), (float)Z_DVAL_PP(g), (float)Z_DVAL_PP(b), (float)Z_DVAL_PP(a),
				   (float)Z_DVAL_PP(width));
}
/* }}} */

/* {{{ proto void swf_shapefilloff(void)
   Turns off filling */
PHP_FUNCTION(swf_shapefilloff)
{
	swf_shapefilloff();
}
/* }}} */

/* {{{ proto void swf_shapefillsolid(float r, float g, float b, float a)
   Sets the current fill style to a solid fill with the specified rgba color */
PHP_FUNCTION(swf_shapefillsolid)
{
	zval **r, **g, **b, **a;
	if (ZEND_NUM_ARGS() != 4 ||
	    zend_get_parameters_ex(4, &r, &g, &b, &a) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(r);
	convert_to_double_ex(g);
	convert_to_double_ex(b);
	convert_to_double_ex(a);
	
	swf_shapefillsolid((float)Z_DVAL_PP(r), (float)Z_DVAL_PP(g), (float)Z_DVAL_PP(b), (float)Z_DVAL_PP(a));
}
/* }}} */

/* {{{ php_swf_fill_bitmap
 */
void php_swf_fill_bitmap(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval **bitmapid;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &bitmapid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(bitmapid);
	
	if (opt) {
		swf_shapefillbitmapclip(Z_LVAL_PP(bitmapid));
	} else {
		swf_shapefillbitmaptile(Z_LVAL_PP(bitmapid));
	}
}
/* }}} */

/* {{{ proto void swf_shapefillbitmapclip(int bitmapid)
   Sets the current fill mode to clipped bitmap fill. Pixels from the previously defined bitmapid will be used to fill areas */
PHP_FUNCTION(swf_shapefillbitmapclip)
{
	php_swf_fill_bitmap(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto void swf_shapefillbitmaptile(int bitmapid)
   Sets the current fill mode to tiled bitmap fill. Pixels from the previously defined bitmapid will be used to fill areas */
PHP_FUNCTION(swf_shapefillbitmaptile)
{
	php_swf_fill_bitmap(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ php_swf_shape
 */
void php_swf_shape(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval **x, **y;
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &x, &y) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	
	if (opt) {
		swf_shapemoveto((float)Z_DVAL_PP(x), (float)Z_DVAL_PP(y));
	} else {
		swf_shapelineto((float)Z_DVAL_PP(x), (float)Z_DVAL_PP(y));
	}
}
/* }}} */

/* {{{ proto void swf_shapemoveto(float x, float y)
   swf_shapemoveto moves the current position to the given x,y. */
PHP_FUNCTION(swf_shapemoveto)
{
	php_swf_shape(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto void swf_shapelineto(float x, float y)
   Draws a line from the current position to x,y, the current position is then set to x,y */
PHP_FUNCTION(swf_shapelineto)
{
	php_swf_shape(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */
 
/* {{{ proto void swf_shapecurveto(float x1, float y1, float x2, float y2)
   Draws a quadratic bezier curve starting at the current position using x1, y1 as an off curve control point and using x2, y2 as the end point. The current position is then set to x2, y2. */
PHP_FUNCTION(swf_shapecurveto)
{
	zval **x1, **y1, **x2, **y2;
	if (ZEND_NUM_ARGS() != 4 ||
	    zend_get_parameters_ex(4, &x1, &y1, &x2, &y2) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x1);
	convert_to_double_ex(y1);
	convert_to_double_ex(x2);
	convert_to_double_ex(y2);
	
	swf_shapecurveto((float)Z_DVAL_PP(x1), (float)Z_DVAL_PP(y1), (float)Z_DVAL_PP(x2), (float)Z_DVAL_PP(y2));
}
/* }}} */

/* {{{ proto void swf_shapecurveto3(float x1, float y1, float x2, float y2, float x3, float y3)
   Draws a cubic bezier curve starting at the current position using x1, y1 and x2, y2 as off curve control points and using x3,y3 as the end point.  The current position is then sent to x3, y3 */
PHP_FUNCTION(swf_shapecurveto3)
{
	zval **x1, **y1, **x2, **y2, **x3, **y3;
	if (ZEND_NUM_ARGS() != 6 ||
	    zend_get_parameters_ex(6, &x1, &y1, &x2, &y2, &x3, &y3) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x1);
	convert_to_double_ex(y1);
	convert_to_double_ex(x2);
	convert_to_double_ex(y2);
	convert_to_double_ex(x3);
	convert_to_double_ex(y3);
	
	swf_shapecurveto3((float)Z_DVAL_PP(x1), (float)Z_DVAL_PP(y1), (float)Z_DVAL_PP(x2), (float)Z_DVAL_PP(y2),
				  (float)Z_DVAL_PP(x3), (float)Z_DVAL_PP(y3));
}
/* }}} */

/* {{{ proto void swf_shapearc(float x, float y, float r, float ang1, float ang2)
   Draws a circular arc from ang1 to ang2. The center of the circle is given by x, and y. r specifies the radius of the arc */
PHP_FUNCTION(swf_shapearc)
{
	zval **x, **y, **r, **ang1, **ang2;
	if (ZEND_NUM_ARGS() != 5 ||
	    zend_get_parameters_ex(5, &x, &y, &r, &ang1, &ang2) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	convert_to_double_ex(r);
	convert_to_double_ex(ang1);
	convert_to_double_ex(ang2);
	
	swf_shapearc((float)Z_DVAL_PP(x), (float)Z_DVAL_PP(y), (float)Z_DVAL_PP(r), (float)Z_DVAL_PP(ang1),
	             (float)Z_DVAL_PP(ang2));
}
/* }}} */

/* {{{ proto void swf_endshape(void)
   Completes the definition of the current shape */
PHP_FUNCTION(swf_endshape)
{
	swf_endshape();
}
/* }}} */

/* {{{ proto void swf_definefont(int fontid, string name)
   Defines a font. name specifies the PostScript name of the font to use. This font also becomes the current font.  */
PHP_FUNCTION(swf_definefont)
{
	zval **fontid, **name;
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &fontid, &name) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fontid);
	convert_to_string_ex(name);
	
	swf_definefont(Z_LVAL_PP(fontid), Z_STRVAL_PP(name));
}
/* }}} */

/* {{{ proto void swf_setfont(int fontid)
   Sets fontid to the current font */
PHP_FUNCTION(swf_setfont)
{
	zval **fontid;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &fontid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fontid);
	swf_setfont(Z_LVAL_PP(fontid));
}
/* }}} */

/* {{{ proto void swf_fontsize(float height)
   Sets the current font's height to the value specified by height */
PHP_FUNCTION(swf_fontsize)
{
	zval **height;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &height) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(height);
	
	swf_fontsize((float)Z_DVAL_PP(height));
}
/* }}} */

/* {{{ proto void swf_fontslant(float slant)
   Set the current font slant to the angle indicated by slant */
PHP_FUNCTION(swf_fontslant)
{
	zval **slant;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &slant) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(slant);
	
	swf_fontslant((float)Z_DVAL_PP(slant));
}
/* }}} */

/* {{{ proto void swf_fonttracking(track)
   Sets the current font tracking to the specified value, track */
PHP_FUNCTION(swf_fonttracking)
{
	zval **track;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &track) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(track);
	swf_fonttracking((float)Z_DVAL_PP(track));
}
/* }}} */

/* {{{ proto array swf_getfontinfo(void)
   Get information about the current font */
PHP_FUNCTION(swf_getfontinfo)
{
	float A_height, x_height;
	swf_getfontinfo(&A_height, &x_height);
	if (array_init(return_value) == FAILURE) {
		php_error(E_WARNING, "Cannot initialize return value from swf_getfontinfo");
		RETURN_FALSE;
	}
	add_assoc_double(return_value, "Aheight", A_height);
	add_assoc_double(return_value, "xheight", x_height);
}
/* }}} */

/* {{{ proto void swf_definetext(int objid, string str, int docCenter)
   defines a text string using the current font, current fontsize and current font slant. If docCenter is 1, the word is centered in x */
PHP_FUNCTION(swf_definetext)
{
	zval **objid, **str, **docCenter;
	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &objid, &str, &docCenter) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	convert_to_string_ex(str);
	convert_to_long_ex(docCenter);
	
	swf_definetext(Z_LVAL_PP(objid), Z_STRVAL_PP(str), Z_LVAL_PP(docCenter));
}
/* }}} */

/* {{{ proto void swf_textwidth(string str)
   Calculates the width of a string, str, using the current fontsize & current font */
PHP_FUNCTION(swf_textwidth)
{
	zval **str;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &str) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	RETURN_DOUBLE((double)swf_textwidth(Z_STRVAL_PP(str)));
}
/* }}} */

/* {{{ proto void swf_definebitmap(int objid, string imgname)
   Defines a bitmap given the name of a .gif .rgb .jpeg or .fi image. The image will be converted into Flash jpeg or Flash color map format */
PHP_FUNCTION(swf_definebitmap)
{
	zval **objid, **imgname;
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &objid, &imgname) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	convert_to_string_ex(imgname);
	
	swf_definebitmap(Z_LVAL_PP(objid), Z_STRVAL_PP(imgname));
}
/* }}} */

/* {{{ proto array swf_getbitmapinfo(int bitmapid)
   Returns an array of information about a bitmap specified by bitmapid */
PHP_FUNCTION(swf_getbitmapinfo)
{
	zval **bitmapid;
	int size, width, height;
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &bitmapid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(bitmapid);
	
	size = swf_getbitmapinfo(Z_LVAL_PP(bitmapid), &width, &height);
	if (array_init(return_value) == FAILURE) {
		php_error(E_WARNING, "Cannot initialize return value from swf_getbitmapinfo");
		RETURN_FALSE;
	}
	
	add_assoc_long(return_value, "size", size);
	add_assoc_long(return_value, "width", width);
	add_assoc_long(return_value, "height", height);
}
/* }}} */

/* {{{ proto void swf_startsymbol(int objid)
   Create a new symbol with object id, objid */
PHP_FUNCTION(swf_startsymbol)
{
	zval **objid;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &objid) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	
	swf_startsymbol(Z_LVAL_PP(objid));
}
/* }}} */

/* {{{ proto void swf_endsymbol(void)
   End the current symbol */
PHP_FUNCTION(swf_endsymbol)
{
	swf_endsymbol();
}
/* }}} */

/* {{{ proto void swf_startbutton(int objid, int type)
   Start a button with an object id, objid and a type of either TYPE_MENUBUTTON or TYPE_PUSHBUTTON */
PHP_FUNCTION(swf_startbutton)
{
	zval **objid, **type;
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &objid, &type) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(objid);
	convert_to_long_ex(type);
	
	swf_startbutton(Z_LVAL_PP(objid), Z_LVAL_PP(type)); /* TYPE_MENUBUTTON, TYPE_PUSHBUTTON */
}
/* }}} */

/* {{{ proto void swf_addbuttonrecord(int state, int objid, int depth)
   Controls the location, appearance and active area of the current button */
PHP_FUNCTION(swf_addbuttonrecord)
{
	zval **state, **objid, **depth;
	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &state, &objid, &depth) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(state);
	convert_to_long_ex(objid);
	convert_to_long_ex(depth);
	
	swf_addbuttonrecord(Z_LVAL_PP(state), Z_LVAL_PP(objid), Z_LVAL_PP(depth));
}
/* }}} */

/* {{{ proto void swf_oncondition(int transitions)
   Describes a transition used to trigger an action list */
PHP_FUNCTION(swf_oncondition)
{
	zval **transitions;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &transitions) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(transitions);
	
	swf_oncondition(Z_LVAL_PP(transitions));
}
/* }}} */

/* {{{ proto void swf_endbutton(void)
   Complete the definition of the current button */
PHP_FUNCTION(swf_endbutton)
{
	swf_endbutton();
}
/* }}} */

/* {{{ php_swf_geo_same
 */
void php_swf_geo_same(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval **arg1, **arg2, **arg3, **arg4;
	if (ZEND_NUM_ARGS() != 4 ||
	    zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(arg1);
	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);

	if (opt == 0) {
		swf_viewport(Z_DVAL_PP(arg1), Z_DVAL_PP(arg2), Z_DVAL_PP(arg3),
		             Z_DVAL_PP(arg4));
	} else if (opt == 1) {
		swf_ortho2(Z_DVAL_PP(arg1), Z_DVAL_PP(arg2), Z_DVAL_PP(arg3),
		             Z_DVAL_PP(arg4));
	} else if (opt == 2) {
		swf_polarview(Z_DVAL_PP(arg1), Z_DVAL_PP(arg2), Z_DVAL_PP(arg3),
		             Z_DVAL_PP(arg4));
	} else if (opt == 3) {
		swf_perspective(Z_DVAL_PP(arg1), Z_DVAL_PP(arg2), Z_DVAL_PP(arg3),
		             Z_DVAL_PP(arg4));
	}
} 
/* }}} */

/* {{{ proto void swf_viewport(float xmin, float xmax, float ymin, float ymax)
   Selects an area on the drawing surface for future drawing */
PHP_FUNCTION(swf_viewport)
{
	php_swf_geo_same(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto void swf_ortho2(float xmin, float xmax, float ymin, float ymax)
   Defines a 2-D orthographic mapping of user coordinates onto the current viewport */ 
PHP_FUNCTION(swf_ortho2)
{
	php_swf_geo_same(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto void swf_ortho(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
   Defines an orthographic mapping of user coordinates onto the current viewport */
PHP_FUNCTION(swf_ortho)
{
	zval **xmin, **xmax, **ymin, **ymax, **zmin, **zmax;
	if (ZEND_NUM_ARGS() != 6 ||
	    zend_get_parameters_ex(6, &xmin, &xmax, &ymin, &ymax, &zmin, &zmax) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(xmin);
	convert_to_double_ex(xmax);
	convert_to_double_ex(ymin);
	convert_to_double_ex(ymax);
	convert_to_double_ex(zmin);
	convert_to_double_ex(zmax);
	
	swf_ortho(Z_DVAL_PP(xmin), Z_DVAL_PP(xmax),
	          Z_DVAL_PP(ymin), Z_DVAL_PP(ymax),
	          Z_DVAL_PP(zmin), Z_DVAL_PP(zmax));
}
/* }}} */

/* {{{ proto void swf_polarview(float dist, float azimuth, float incidence, float twist)
   Defines he viewer's position in polar coordinates */
PHP_FUNCTION(swf_polarview)
{
	php_swf_geo_same(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto void swf_perspective(float fovy, float aspect, float near, float far)
   Define a perspective projection transformation. */
PHP_FUNCTION(swf_perspective)
{
	php_swf_geo_same(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ proto void swf_lookat(float vx, float vy, float vz, float px, float py, float pz, float twist)
   Defines a viewing transformation by giving the view position vx, vy, vz, and the coordinates of a reference point in the scene at px, py, pz. Twist controls a rotation along the viewer's z axis */
PHP_FUNCTION(swf_lookat)
{
	zval **vx, **vy, **vz, **px, **py, **pz, **twist;
	if (ZEND_NUM_ARGS() != 7 ||
	    zend_get_parameters_ex(7, &vx, &vy, &vz, &px, &py, &pz, &twist) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(vx);
	convert_to_double_ex(vy);
	convert_to_double_ex(vz);
	convert_to_double_ex(px);
	convert_to_double_ex(py);
	convert_to_double_ex(pz);
	convert_to_double_ex(twist);
	
	swf_lookat(Z_DVAL_PP(vx), Z_DVAL_PP(vy), Z_DVAL_PP(vz),
	           Z_DVAL_PP(px), Z_DVAL_PP(py), Z_DVAL_PP(pz), Z_DVAL_PP(twist));
}
/* }}} */

/* {{{ proto void swf_pushmatrix(void)
   Push the current transformation matrix onto the stack */
PHP_FUNCTION(swf_pushmatrix)
{
	swf_pushmatrix();
}
/* }}} */

/* {{{ proto void swf_popmatrix(void)
   Restore a previous transformation matrix */
PHP_FUNCTION(swf_popmatrix)
{
	swf_popmatrix();
}
/* }}} */

/* {{{ proto void swf_scale(float x, float y, float z)
   Scale the current transformation */
PHP_FUNCTION(swf_scale)
{
	zval **x, **y, **z;
	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &x, &y, &z) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	convert_to_double_ex(z);
	
	swf_scale(Z_DVAL_PP(x), Z_DVAL_PP(y), Z_DVAL_PP(z));
}
/* }}} */

/* {{{ proto void swf_translate(float x, float y, float z)
   Translate the current transformation */
PHP_FUNCTION(swf_translate)
{
	zval **x, **y, **z;
	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &x, &y, &z) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(x);
	convert_to_double_ex(y);
	convert_to_double_ex(z);
	
	swf_translate(Z_DVAL_PP(x), Z_DVAL_PP(y), Z_DVAL_PP(z));
}
/* }}} */

/* {{{ proto void swf_rotate(float angle, string axis)
   Rotate the current transformation by the given angle about x, y, or z axis. The axis may be 'x', 'y', or 'z' */
PHP_FUNCTION(swf_rotate)
{
	zval **angle, **axis;
	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &angle, &axis) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(angle);
	convert_to_string_ex(axis);

	swf_rotate(Z_DVAL_PP(angle), (char)(Z_STRVAL_PP(axis))[0]);
}
/* }}} */

/* {{{ proto void swf_posround(int doit)
   This enables or disables rounding of the translation when objects are places or moved */
PHP_FUNCTION(swf_posround)
{
	zval **doit;
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &doit) == FAILURE) {
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(doit);
	
	swf_posround(Z_LVAL_PP(doit));
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
