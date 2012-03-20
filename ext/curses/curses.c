/* -*- C -*-
 * $Id$
 *
 * ext/curses/curses.c
 *
 * by MAEDA Shugo (ender@pic-internet.or.jp)
 * modified by Yukihiro Matsumoto (matz@netlab.co.jp),
 *         Toki Yoshinori,
 *         Hitoshi Takahashi,
 *         and Takaaki Tateishi (ttate@kt.jaist.ac.jp)
 *
 * maintainers:
 * - Takaaki Tateishi (ttate@kt.jaist.ac.jp)
 *
 * doumentation:
 * - Vincent Batts (vbatts@hashbangbash.com)
 */

#include "ruby.h"
#include "ruby/io.h"

#if defined(HAVE_NCURSES_H)
# include <ncurses.h>
#elif defined(HAVE_NCURSES_CURSES_H)
# include <ncurses/curses.h>
#elif defined(HAVE_CURSES_COLR_CURSES_H)
# ifdef HAVE_STDARG_PROTOTYPES
#  include <stdarg.h>
# else
#  include <varargs.h>
# endif
# include <curses_colr/curses.h>
#else
# include <curses.h>
# if defined(__bsdi__) || defined(__NetBSD__) || defined(__APPLE__)
#  if !defined(_maxx)
#  define _maxx maxx
#  endif
#  if !defined(_maxy)
#  define _maxy maxy
#  endif
#  if !defined(_begx)
#  define _begx begx
#  endif
#  if !defined(_begy)
#  define _begy begy
#  endif
# endif
#endif

#ifdef HAVE_INIT_COLOR
# define USE_COLOR 1
#endif

/* supports only ncurses mouse routines */
#ifdef NCURSES_MOUSE_VERSION
# define USE_MOUSE 1
#endif

#define NUM2CH NUM2CHR
#define CH2FIX CHR2FIX

static VALUE mCurses;
static VALUE mKey;
static VALUE cWindow;
static VALUE cPad;
#ifdef USE_MOUSE
static VALUE cMouseEvent;
#endif

static VALUE rb_stdscr;

struct windata {
    WINDOW *window;
};

static VALUE window_attroff(VALUE obj, VALUE attrs);
static VALUE window_attron(VALUE obj, VALUE attrs);
static VALUE window_attrset(VALUE obj, VALUE attrs);

static void
no_window(void)
{
    rb_raise(rb_eRuntimeError, "already closed window");
}

#define GetWINDOW(obj, winp) do {\
    if (!OBJ_TAINTED(obj) && rb_safe_level() >= 4)\
	rb_raise(rb_eSecurityError, "Insecure: operation on untainted window");\
    TypedData_Get_Struct((obj), struct windata, &windata_type, (winp));\
    if ((winp)->window == 0) no_window();\
} while (0)

static void
window_free(void *p)
{
    struct windata *winp = p;
    if (winp->window && winp->window != stdscr) delwin(winp->window);
    winp->window = 0;
    xfree(winp);
}

static size_t
window_memsize(const void *p)
{
    const struct windata *winp = p;
    size_t size = sizeof(*winp);
    if (!winp) return 0;
    if (winp->window && winp->window != stdscr) size += sizeof(winp->window);
    return size;
}

static const rb_data_type_t windata_type = {
    "windata",
    {0, window_free, window_memsize,}
};

static VALUE
prep_window(VALUE class, WINDOW *window)
{
    VALUE obj;
    struct windata *winp;

    if (window == NULL) {
	rb_raise(rb_eRuntimeError, "failed to create window");
    }

    obj = rb_obj_alloc(class);
    TypedData_Get_Struct(obj, struct windata, &windata_type, winp);
    winp->window = window;

    return obj;
}

/*-------------------------- module Curses --------------------------*/

/*
 * Document-method: Curses.init_screen
 *
 * Initialize a standard screen
 *
 * see also Curses.stdscr
 */
static VALUE
curses_init_screen(void)
{
    rb_secure(4);
    if (rb_stdscr) return rb_stdscr;
    initscr();
    if (stdscr == 0) {
	rb_raise(rb_eRuntimeError, "can't initialize curses");
    }
    clear();
    rb_stdscr = prep_window(cWindow, stdscr);
    return rb_stdscr;
}

/*
 * Document-method: Curses.stdscr
 *
 * The Standard Screen.
 *
 * Upon initializing curses, a default window called stdscr,
 * which is the size of the terminal screen, is created.
 *
 * Many curses functions use this window.
 */
#define curses_stdscr curses_init_screen

/*
 * Document-method: Curses.close_screen
 *
 * A program should always call Curses.close_screen before exiting or
 * escaping from curses mode temporarily. This routine
 * restores tty modes, moves the cursor to the lower
 * left-hand corner of the screen and resets the terminal
 * into the proper non-visual mode.
 *
 * Calling Curses.refresh or Curses.doupdate after a temporary
 * escape causes the program to resume visual mode.
 *
 */
static VALUE
curses_close_screen(void)
{
    curses_stdscr();
#ifdef HAVE_ISENDWIN
    if (!isendwin())
#endif
	endwin();
    rb_stdscr = 0;
    return Qnil;
}

/*
 * This is no runtime method,
 * but a function called before the proc ends
 *
 * Similar to Curses.close_screen, except that it also
 * garbage collects/unregisters the Curses.stdscr
 */
static void
curses_finalize(VALUE dummy)
{
    if (stdscr
#ifdef HAVE_ISENDWIN
	&& !isendwin()
#endif
	)
	endwin();
    rb_stdscr = 0;
    rb_gc_unregister_address(&rb_stdscr);
}

#ifdef HAVE_ISENDWIN
/*
 * Document-method: Curses.closed?
 *
 * Returns +true+ if the window/screen has been closed,
 * without any subsequent Curses.refresh calls,
 * returns +false+ otherwise.
 */
static VALUE
curses_closed(void)
{
    curses_stdscr();
    if (isendwin()) {
	return Qtrue;
    }
    return Qfalse;
}
#else
#define curses_closed rb_f_notimplement
#endif

/*
 * Document-method: Curses.clear
 *
 * Clears every position on the screen completely,
 * so that a subsequent call by Curses.refresh for the screen/window
 * will be repainted from scratch.
 */
static VALUE
curses_clear(VALUE obj)
{
    curses_stdscr();
    wclear(stdscr);
    return Qnil;
}

/*
 * Document-method: Curses.clrtoeol
 *
 * Clears to the end of line, that the cursor is currently on.
 */
static VALUE
curses_clrtoeol(void)
{
    curses_stdscr();
    clrtoeol();
    return Qnil;
}

/*
 * Document-method: Curses.refresh
 *
 * Refreshes the windows and lines.
 *
 */
static VALUE
curses_refresh(VALUE obj)
{
    curses_stdscr();
    refresh();
    return Qnil;
}

/*
 * Document-method: Curses.doupdate
 *
 * Refreshes the windows and lines.
 *
 * Curses.doupdate allows multiple updates with
 * more efficiency than Curses.refresh alone.
 */
static VALUE
curses_doupdate(VALUE obj)
{
    curses_stdscr();
#ifdef HAVE_DOUPDATE
    doupdate();
#else
    refresh();
#endif
    return Qnil;
}

/*
 * Document-method: Curses.echo
 *
 * Enables characters typed by the user
 * to be echoed by Curses.getch as they are typed.
 */
static VALUE
curses_echo(VALUE obj)
{
    curses_stdscr();
    echo();
    return Qnil;
}

/*
 * Document-method: Curses.noecho
 *
 * Disables characters typed by the user
 * to be echoed by Curses.getch as they are typed.
 */
static VALUE
curses_noecho(VALUE obj)
{
    curses_stdscr();
    noecho();
    return Qnil;
}

/*
 * Document-method: Curses.raw
 *
 * Put the terminal into raw mode.
 *
 * Raw mode is similar to Curses.cbreak mode, in that characters typed
 * are immediately passed through to the user program.
 *
 * The differences are that in raw mode, the interrupt, quit,
 * suspend, and flow control characters are all passed through
 * uninterpreted, instead of generating a signal. The behavior
 * of the BREAK key depends on other bits in the tty driver
 * that are not set by curses.
 */
static VALUE
curses_raw(VALUE obj)
{
    curses_stdscr();
    raw();
    return Qnil;
}

/*
 * Document-method: Curses.noraw
 *
 * Put the terminal out of raw mode.
 *
 * see Curses.raw for more detail
 */
static VALUE
curses_noraw(VALUE obj)
{
    curses_stdscr();
    noraw();
    return Qnil;
}

/*
 * Document-method: Curses.cbreak
 *
 * Put the terminal into cbreak mode.
 *
 * Normally, the tty driver buffers typed characters until
 * a newline or carriage return is typed. The Curses.cbreak
 * routine disables line buffering and erase/kill
 * character-processing (interrupt and flow control characters
 * are unaffected), making characters typed by the user
 * immediately available to the program.
 *
 * The Curses.nocbreak routine returns the terminal to normal (cooked) mode.
 *
 * Initially the terminal may or may not be in cbreak mode,
 * as the mode is inherited; therefore, a program should
 * call Curses.cbreak or Curses.nocbreak explicitly.
 * Most interactive programs using curses set the cbreak mode.
 * Note that Curses.cbreak overrides Curses.raw.
 *
 * see also Curses.raw
 */
static VALUE
curses_cbreak(VALUE obj)
{
    curses_stdscr();
    cbreak();
    return Qnil;
}

/*
 * Document-method: Curses.nocbreak
 *
 * Put the terminal into normal mode (out of cbreak mode).
 *
 * See Curses.cbreak for more detail.
 */
static VALUE
curses_nocbreak(VALUE obj)
{
    curses_stdscr();
    nocbreak();
    return Qnil;
}

/*
 * Document-method: Curses.nl
 *
 * Enable the underlying display device to translate
 * the return key into newline on input, and whether it
 * translates newline into return and line-feed on output
 * (in either case, the call Curses.addch('\n') does the
 * equivalent of return and line feed on the virtual screen).
 *
 * Initially, these translations do occur. If you disable
 * them using Curses.nonl, curses will be able to make better use
 * of the line-feed capability, resulting in faster cursor
 * motion. Also, curses will then be able to detect the return key.
 */
static VALUE
curses_nl(VALUE obj)
{
    curses_stdscr();
    nl();
    return Qnil;
}

/*
 * Document-method: Curses.nl
 *
 * Disable the underlying display device to translate
 * the return key into newline on input
 *
 * See Curses.nl for more detail
 */
static VALUE
curses_nonl(VALUE obj)
{
    curses_stdscr();
    nonl();
    return Qnil;
}

/*
 * Document-method: Curses.beep
 *
 * Sounds an audible alarm on the terminal, if possible;
 * otherwise it flashes the screen (visual bell).
 *
 * see also Curses.flash
 */
static VALUE
curses_beep(VALUE obj)
{
#ifdef HAVE_BEEP
    curses_stdscr();
    beep();
#endif
    return Qnil;
}

/*
 * Document-method: Curses.flash
 *
 * Flashs the screen, for visual alarm on the terminal, if possible;
 * otherwise it sounds the alert.
 *
 * see also Curses.beep
 */
static VALUE
curses_flash(VALUE obj)
{
#ifdef HAVE_FLASH
    curses_stdscr();
    flash();
#endif
    return Qnil;
}

static int
curses_char(VALUE c)
{
    if (FIXNUM_P(c)) {
	return NUM2INT(c);
    }
    else {
	int cc;

	StringValue(c);
	if (RSTRING_LEN(c) == 0 || RSTRING_LEN(c) > 1) {
	    rb_raise(rb_eArgError, "string not corresponding a character");
	}
	cc = RSTRING_PTR(c)[0];
	if (cc > 0x7f) {
	    rb_raise(rb_eArgError, "no multibyte string supported (yet)");
	}
	return cc;
    }
}

#ifdef HAVE_UNGETCH
/*
 * Document-method: Curses.ungetch
 * call-seq: ungetch(ch)
 *
 * Places +ch+ back onto the input queue to be returned by
 * the next call to Curses.getch.
 *
 * There is just one input queue for all windows.
 */
static VALUE
curses_ungetch(VALUE obj, VALUE ch)
{
    int c = curses_char(ch);
    curses_stdscr();
    ungetch(c);
    return Qnil;
}
#else
#define curses_ungetch rb_f_notimplement
#endif

/*
 * Document-method: Curses.setpos
 * call-seq: setpos(y, x)
 *
 * A setter for the position of the cursor,
 * using coordinates +x+ and +y+
 *
 */
static VALUE
curses_setpos(VALUE obj, VALUE y, VALUE x)
{
    curses_stdscr();
    move(NUM2INT(y), NUM2INT(x));
    return Qnil;
}

/*
 * Document-method: Curses.standout
 *
 * Enables the best highlighting mode of the terminal.
 *
 * This is equivalent to Curses:Window.attron(A_STANDOUT)
 *
 * see also Curses::Window.attrset additional information
 */
static VALUE
curses_standout(VALUE obj)
{
    curses_stdscr();
    standout();
    return Qnil;
}

/*
 * Document-method: Curses.standend
 *
 * Enables the Normal display (no highlight)
 *
 * This is equivalent to Curses.attron(A_NORMAL)
 *
 * see also Curses::Window.attrset for additional information.
 */
static VALUE
curses_standend(VALUE obj)
{
    curses_stdscr();
    standend();
    return Qnil;
}

/*
 * Document-method: Curses.inch
 *
 * Returns the character at the current position.
 */
static VALUE
curses_inch(VALUE obj)
{
    curses_stdscr();
    return CH2FIX(inch());
}

/*
 * Document-method: Curses.addch
 * call-seq: addch(ch)
 *
 * Add a character +ch+, with attributes, then advance the cursor.
 *
 * see also the system manual for curs_addch(3)
 */
static VALUE
curses_addch(VALUE obj, VALUE ch)
{
    curses_stdscr();
    addch(NUM2CH(ch));
    return Qnil;
}

/*
 * Document-method: Curses.insch
 * call-seq: insch(ch)
 *
 * Insert a character +ch+, before the cursor.
 *
 */
static VALUE
curses_insch(VALUE obj, VALUE ch)
{
    curses_stdscr();
    insch(NUM2CH(ch));
    return Qnil;
}

/*
 * Document-method: Curses.addstr
 * call-seq: addstr(str)
 *
 * add a string of characters +str+, to the window and advance cursor
 *
 */
static VALUE
curses_addstr(VALUE obj, VALUE str)
{
    StringValue(str);
    str = rb_str_export_locale(str);
    curses_stdscr();
    if (!NIL_P(str)) {
	addstr(StringValueCStr(str));
    }
    return Qnil;
}

static VALUE
getch_func(void *arg)
{
    int *ip = (int *)arg;
    *ip = getch();
    return Qnil;
}

/*
 * Document-method: Curses.getch
 *
 * Read and returns a character from the window.
 *
 * See Curses::Key to all the function KEY_* available
 *
 */
static VALUE
curses_getch(VALUE obj)
{
    int c;

    curses_stdscr();
    rb_thread_blocking_region(getch_func, (void *)&c, RUBY_UBF_IO, 0);
    if (c == EOF) return Qnil;
    if (rb_isprint(c)) {
	char ch = (char)c;

	return rb_locale_str_new(&ch, 1);
    }
    return UINT2NUM(c);
}

/* This should be big enough.. I hope */
#define GETSTR_BUF_SIZE 1024

static VALUE
getstr_func(void *arg)
{
    char *rtn = (char *)arg;
#if defined(HAVE_GETNSTR)
    getnstr(rtn,GETSTR_BUF_SIZE-1);
#else
    getstr(rtn);
#endif
    return Qnil;
}

/*
 * Document-method: Curses.getstr
 *
 * This is equivalent to a series f Curses::Window.getch calls
 *
 */
static VALUE
curses_getstr(VALUE obj)
{
    char rtn[GETSTR_BUF_SIZE];

    curses_stdscr();
    rb_thread_blocking_region(getstr_func, (void *)rtn, RUBY_UBF_IO, 0);
    return rb_locale_str_new_cstr(rtn);
}

/*
 * Document-method: Curses.delch
 *
 * Delete the character under the cursor
 *
 */
static VALUE
curses_delch(VALUE obj)
{
    curses_stdscr();
    delch();
    return Qnil;
}

/*
 * Document-method: Curses.deleteln
 *
 * Delete the line under the cursor.
 *
 */
static VALUE
curses_deleteln(VALUE obj)
{
    curses_stdscr();
#if defined(HAVE_DELETELN) || defined(deleteln)
    deleteln();
#endif
    return Qnil;
}

/*
 * Document-method: Curses.insertln
 *
 * Inserts a line above the cursor, and the bottom line is lost
 *
 */
static VALUE
curses_insertln(VALUE obj)
{
    curses_stdscr();
#if defined(HAVE_INSERTLN) || defined(insertln)
    insertln();
#endif
    return Qnil;
}

/*
 * Document-method: Curses.keyname
 * call-seq: keyname(c)
 *
 * Returns the character string corresponding to key +c+
 */
static VALUE
curses_keyname(VALUE obj, VALUE c)
{
#ifdef HAVE_KEYNAME
    int cc = curses_char(c);
    const char *name;

    curses_stdscr();
    name = keyname(cc);
    if (name) {
	return rb_str_new_cstr(name);
    }
    else {
	return Qnil;
    }
#else
    return Qnil;
#endif
}

/*
 * Document-method: Curses.lines
 *
 * Returns the number of lines on the screen
 */
static VALUE
curses_lines(void)
{
    return INT2FIX(LINES);
}

/*
 * Document-method: Curses.cols
 *
 * Returns the number of columns on the screen
 */
static VALUE
curses_cols(void)
{
    return INT2FIX(COLS);
}

/*
 * Document-method: Curses.curs_set
 * call-seq: curs_set(visibility)
 *
 * Sets Cursor Visibility.
 * 0: invisible
 * 1: visible
 * 2: very visible
 */
static VALUE
curses_curs_set(VALUE obj, VALUE visibility)
{
#ifdef HAVE_CURS_SET
    int n;
    curses_stdscr();
    return (n = curs_set(NUM2INT(visibility)) != ERR) ? INT2FIX(n) : Qnil;
#else
    return Qnil;
#endif
}

/*
 * Document-method: Curses.scrl
 * call-seq: scrl(num)
 *
 * Scrolls the current window Fixnum +num+ lines.
 * The current cursor position is not changed.
 *
 * For positive +num+, it scrolls up.
 *
 * For negative +num+, it scrolls down.
 *
 */
static VALUE
curses_scrl(VALUE obj, VALUE n)
{
    /* may have to raise exception on ERR */
#ifdef HAVE_SCRL
    curses_stdscr();
    return (scrl(NUM2INT(n)) == OK) ? Qtrue : Qfalse;
#else
    return Qfalse;
#endif
}

/*
 * Document-method: Curses.setscrreg
 *
 * call-seq:
 *   setscrreg(top, bottom)
 *
 * Set a software scrolling region in a window.
 * +top+ and +bottom+ are lines numbers of the margin.
 *
 * If this option and Curses.scrollok are enabled, an attempt to move off
 * the bottom margin line causes all lines in the scrolling region
 * to scroll one line in the direction of the first line.
 * Only the text of the window is scrolled.
 *
 */
static VALUE
curses_setscrreg(VALUE obj, VALUE top, VALUE bottom)
{
    /* may have to raise exception on ERR */
#ifdef HAVE_SETSCRREG
    curses_stdscr();
    return (setscrreg(NUM2INT(top), NUM2INT(bottom)) == OK) ? Qtrue : Qfalse;
#else
    return Qfalse;
#endif
}

/*
 * Document-method: Curses.attroff
 * call-seq: attroff(attrs)
 *
 * Turns on the named attributes +attrs+ without affecting any others.
 *
 * See also Curses::Window.attrset for additional information.
 */
static VALUE
curses_attroff(VALUE obj, VALUE attrs)
{
    curses_stdscr();
    return window_attroff(rb_stdscr,attrs);
    /* return INT2FIX(attroff(NUM2INT(attrs))); */
}

/*
 * Document-method: Curses.attron
 * call-seq: attron(attrs)
 *
 * Turns off the named attributes +attrs+
 * without turning any other attributes on or off.
 *
 * See also Curses::Window.attrset for additional information.
 */
static VALUE
curses_attron(VALUE obj, VALUE attrs)
{
    curses_stdscr();
    return window_attron(rb_stdscr,attrs);
    /* return INT2FIX(attroff(NUM2INT(attrs))); */
}

/*
 * Document-method: Curses.attrset
 * call-seq: attrset(attrs)
 *
 * Sets the current attributes of the given window to +attrs+.
 *
 * see also Curses::Window.attrset
 *
 */
static VALUE
curses_attrset(VALUE obj, VALUE attrs)
{
    curses_stdscr();
    return window_attrset(rb_stdscr,attrs);
    /* return INT2FIX(attroff(NUM2INT(attrs))); */
}

/*
 * Document-method: Curses.bkgdset
 * call-seq: bkgdset(ch)
 *
 * Manipulate the background of the named window
 * with character Integer +ch+
 *
 * The background becomes a property of the character
 * and moves with the character through any scrolling
 * and insert/delete line/character operations.
 *
 * see also the system manual for curs_bkgd(3)
 */
static VALUE
curses_bkgdset(VALUE obj, VALUE ch)
{
#ifdef HAVE_BKGDSET
    curses_stdscr();
    bkgdset(NUM2CH(ch));
#endif
    return Qnil;
}

/*
 * call-seq: bkgd(ch)
 *
 * Window background manipulation routines.
 *
 * Set the background property of the current
 * and then apply the character Integer +ch+ setting
 * to every character position in that window.
 *
 * see also the system manual for curs_bkgd(3)
 */
static VALUE
curses_bkgd(VALUE obj, VALUE ch)
{
#ifdef HAVE_BKGD
    curses_stdscr();
    return (bkgd(NUM2CH(ch)) == OK) ? Qtrue : Qfalse;
#else
    return Qfalse;
#endif
}

#if defined(HAVE_USE_DEFAULT_COLORS)
/*
 * tells the curses library to use terminal's default colors.
 *
 * see also the system manual for default_colors(3)
 */
static VALUE
curses_use_default_colors(VALUE obj)
{
    curses_stdscr();
    use_default_colors();
    return Qnil;
}
#else
#define curses_use_default_colors rb_f_notimplement
#endif

#if defined(HAVE_TABSIZE)
/*
 * Document-method: Curses.TABSIZE=
 * call-seq: TABSIZE=(value)
 *
 * Sets the TABSIZE to Integer +value+
 */
static VALUE
curses_tabsize_set(VALUE obj, VALUE val)
{
    TABSIZE = NUM2INT(val);
    return INT2NUM(TABSIZE);
}
#else
#define curses_tabsize_set rb_f_notimplement
#endif

#if defined(HAVE_TABSIZE)
/*
 * Returns the number of positions in a tab.
 */
static VALUE
curses_tabsize_get(VALUE ojb)
{
    return INT2NUM(TABSIZE);
}
#else
#define curses_tabsize_get rb_f_notimplement
#endif

#if defined(HAVE_ESCDELAY)
/*
 * call-seq: Curses.ESCDELAY=(value)
 *
 * Sets the ESCDELAY to Integer +value+
 */
static VALUE
curses_escdelay_set(VALUE obj, VALUE val)
{
    ESCDELAY = NUM2INT(val);
    return INT2NUM(ESCDELAY);
}
#else
#define curses_escdelay_set rb_f_notimplement
#endif

#if defined(HAVE_ESCDELAY)
/*
 * Returns the total time, in milliseconds, for which
 * curses will await a character sequence, e.g., a function key
 */
static VALUE
curses_escdelay_get(VALUE obj)
{
    return INT2NUM(ESCDELAY);
}
#else
#define curses_escdelay_get rb_f_notimplement
#endif

/*
 * Document-method: Curses.resize
 * call-seq: resize(lines, cols)
 *
 * alias for Curses.resizeterm
 *
 */

/*
 * Document-method: Curses.resizeterm
 * call-seq: resizeterm(lines, cols)
 *
 * Resize the current term to Fixnum +lines+ and Fixnum +cols+
 *
 */
static VALUE
curses_resizeterm(VALUE obj, VALUE lin, VALUE col)
{
#if defined(HAVE_RESIZETERM)
    curses_stdscr();
    return (resizeterm(NUM2INT(lin),NUM2INT(col)) == OK) ? Qtrue : Qfalse;
#else
    return Qnil;
#endif
}

#ifdef USE_COLOR
/*
 * Document-method: Curses.start_color
 *
 * Initializes the color attributes, for terminals that support it.
 *
 * This must be called, in order to use color attributes.
 * It is good practice to call it just after Curses.init_screen
 */
static VALUE
curses_start_color(VALUE obj)
{
    /* may have to raise exception on ERR */
    curses_stdscr();
    return (start_color() == OK) ? Qtrue : Qfalse;
}

/*
 * Document-method: Curses.init_pair
 * call-seq: init_pair(pair, f, b)
 *
 * Changes the definition of a color-pair.
 *
 * It takes three arguments: the number of the color-pair to be changed +pair+,
 * the foreground color number +f+, and the background color number +b+.
 *
 * If the color-pair was previously initialized, the screen is
 * refreshed and all occurrences of that color-pair are changed
 * to the new definition.
 *
 */
static VALUE
curses_init_pair(VALUE obj, VALUE pair, VALUE f, VALUE b)
{
    /* may have to raise exception on ERR */
    curses_stdscr();
    return (init_pair(NUM2INT(pair),NUM2INT(f),NUM2INT(b)) == OK) ? Qtrue : Qfalse;
}

/*
 * Document-method: Curses.init_color
 * call-seq: init_color(color, r, g, b)
 *
 * Changes the definition of a color. It takes four arguments:
 * * the number of the color to be changed, +color+
 * * the amount of red, +r+
 * * the amount of green, +g+
 * * the amount of blue, +b+
 *
 * The value of the first argument must be between 0 and  COLORS.
 * (See the section Colors for the default color index.)  Each
 * of the last three arguments must be a value between 0 and 1000.
 * When Curses.init_color is used, all occurrences of that color
 * on the screen immediately change to the new definition.
 */
static VALUE
curses_init_color(VALUE obj, VALUE color, VALUE r, VALUE g, VALUE b)
{
    /* may have to raise exception on ERR */
    curses_stdscr();
    return (init_color(NUM2INT(color),NUM2INT(r),
		       NUM2INT(g),NUM2INT(b)) == OK) ? Qtrue : Qfalse;
}

/*
 * Document-method: Curses.has_colors?
 *
 * Returns +true+ or +false+ depending on whether the terminal has color capbilities.
 */
static VALUE
curses_has_colors(VALUE obj)
{
    curses_stdscr();
    return has_colors() ? Qtrue : Qfalse;
}

/*
 * Document-method: Curses.can_change_color?
 *
 * Returns +true+ or +false+ depending on whether the terminal can change color attributes
 */
static VALUE
curses_can_change_color(VALUE obj)
{
    curses_stdscr();
    return can_change_color() ? Qtrue : Qfalse;
}

#if defined(HAVE_COLORS)
/*
 * Document-method: Curses.color
 *
 * returns COLORS
 */
static VALUE
curses_colors(VALUE obj)
{
    return INT2FIX(COLORS);
}
#else
#define curses_colors rb_f_notimplement
#endif

/*
 * Document-method: Curses.color_content
 * call-seq: color_content(color)
 *
 * Returns an 3 item Array of the RGB values in +color+
 */
static VALUE
curses_color_content(VALUE obj, VALUE color)
{
    short r,g,b;

    curses_stdscr();
    color_content(NUM2INT(color),&r,&g,&b);
    return rb_ary_new3(3,INT2FIX(r),INT2FIX(g),INT2FIX(b));
}


#if defined(HAVE_COLOR_PAIRS)
/*
 * Document-method: Curses.color_pairs
 *
 * Returns the COLOR_PAIRS available, if the curses library supports it.
 */
static VALUE
curses_color_pairs(VALUE obj)
{
    return INT2FIX(COLOR_PAIRS);
}
#else
#define curses_color_pairs rb_f_notimplement
#endif

/*
 * Document-method: Curses.pair_content
 * call-seq: pair_content(pair)
 *
 * Returns a 2 item Array, with the foreground and
 * background color, in +pair+
 */
static VALUE
curses_pair_content(VALUE obj, VALUE pair)
{
    short f,b;

    curses_stdscr();
    pair_content(NUM2INT(pair),&f,&b);
    return rb_ary_new3(2,INT2FIX(f),INT2FIX(b));
}

/*
 * Document-method: Curses.color_pair
 * call-seq: color_pair(attrs)
 *
 * Sets the color pair attributes to +attrs+.
 *
 * This should be equivalent to Curses.attrset(COLOR_PAIR(+attrs+))
 *
 * TODO: validate that equivalency
 */
static VALUE
curses_color_pair(VALUE obj, VALUE attrs)
{
    return INT2FIX(COLOR_PAIR(NUM2INT(attrs)));
}

/*
 * Document-method: Curses.pair_number
 * call-seq: pair_number(attrs)
 *
 * Returns the Fixnum color pair number of attributes +attrs+.
 */
static VALUE
curses_pair_number(VALUE obj, VALUE attrs)
{
    curses_stdscr();
    return INT2FIX(PAIR_NUMBER(NUM2INT(attrs)));
}
#endif /* USE_COLOR */

#ifdef USE_MOUSE
struct mousedata {
    MEVENT *mevent;
};

static void
no_mevent(void)
{
    rb_raise(rb_eRuntimeError, "no such mouse event");
}

#define GetMOUSE(obj, data) do {\
    if (!OBJ_TAINTED(obj) && rb_safe_level() >= 4)\
	rb_raise(rb_eSecurityError, "Insecure: operation on untainted mouse");\
    TypedData_Get_Struct((obj), struct mousedata, &mousedata_type, (data));\
    if ((data)->mevent == 0) no_mevent();\
} while (0)

static void
curses_mousedata_free(void *p)
{
    struct mousedata *mdata = p;
    if (mdata->mevent)
	xfree(mdata->mevent);
}

static size_t
curses_mousedata_memsize(const void *p)
{
    const struct mousedata *mdata = p;
    size_t size = sizeof(*mdata);
    if (!mdata) return 0;
    if (mdata->mevent) size += sizeof(mdata->mevent);
    return size;
}

static const rb_data_type_t mousedata_type = {
    "mousedata",
    {0, curses_mousedata_free, curses_mousedata_memsize,}
};

/*
 * Document-method: Curses.getmouse
 *
 * Returns coordinates of the mouse.
 *
 * This will read and pop the mouse event data off the queue
 *
 * See the BUTTON*, ALL_MOUSE_EVENTS and REPORT_MOUSE_POSITION constants, to examine the mask of the event
 */
static VALUE
curses_getmouse(VALUE obj)
{
    struct mousedata *mdata;
    VALUE val;

    curses_stdscr();
    val = TypedData_Make_Struct(cMouseEvent,struct mousedata,
				&mousedata_type,mdata);
    mdata->mevent = (MEVENT*)xmalloc(sizeof(MEVENT));
    return (getmouse(mdata->mevent) == OK) ? val : Qnil;
}

/*
 * Document-method: Curses.ungetmouse
 *
 * It pushes a KEY_MOUSE event onto the input queue, and associates with that
 * event the given state data and screen-relative character-cell coordinates.
 *
 * The Curses.ungetmouse function behaves analogously to Curses.ungetch.
 */
static VALUE
curses_ungetmouse(VALUE obj, VALUE mevent)
{
    struct mousedata *mdata;

    curses_stdscr();
    GetMOUSE(mevent,mdata);
    return (ungetmouse(mdata->mevent) == OK) ? Qtrue : Qfalse;
}

/*
 * Document-method: Curses.mouseinterval
 * call-seq: mouseinterval(interval)
 *
 * The Curses.mouseinterval function sets the maximum time
 * (in thousands of a second) that can elapse between press
 * and release events for them to be recognized as a click.
 *
 * Use Curses.mouseinterval(0) to disable click resolution.
 * This function returns the previous interval value.
 *
 * Use Curses.mouseinterval(-1) to obtain the interval without
 * altering it.
 *
 * The default is one sixth of a second.
 */
static VALUE
curses_mouseinterval(VALUE obj, VALUE interval)
{
    curses_stdscr();
    return mouseinterval(NUM2INT(interval)) ? Qtrue : Qfalse;
}

/*
 * Document-method: Curses.mousemask
 * call-seq: mousemask(mask)
 *
 * Returns the +mask+ of the reportable events
 */
static VALUE
curses_mousemask(VALUE obj, VALUE mask)
{
    curses_stdscr();
    return INT2NUM(mousemask(NUM2UINT(mask),NULL));
}

#define DEFINE_MOUSE_GET_MEMBER(func_name,mem) \
static VALUE func_name (VALUE mouse) \
{ \
    struct mousedata *mdata; \
    GetMOUSE(mouse, mdata); \
    return (UINT2NUM(mdata->mevent -> mem)); \
}

/*
 * Document-method: Curses::MouseEvent.eid
 *
 * Returns the current mouse id
 */
DEFINE_MOUSE_GET_MEMBER(curs_mouse_id, id)
/*
 * Document-method: Curses::MouseEvent.x
 *
 * Returns the current mouse's X coordinate
 */
DEFINE_MOUSE_GET_MEMBER(curs_mouse_x, x)
/*
 * Document-method: Curses::MouseEvent.y
 *
 * Returns the current mouse's Y coordinate
 */
DEFINE_MOUSE_GET_MEMBER(curs_mouse_y, y)
/*
 * Document-method: Curses::MouseEvent.z
 *
 * Returns the current mouse's Z coordinate
 */
DEFINE_MOUSE_GET_MEMBER(curs_mouse_z, z)
/*
 * Document-method: Curses::MouseEvent.bstate
 *
 * Returns the current mouse's button state.  Use this with the button state
 * constants to determine which buttons were pressed.
 */
DEFINE_MOUSE_GET_MEMBER(curs_mouse_bstate, bstate)
#undef define_curs_mouse_member
#endif /* USE_MOUSE */

#ifdef HAVE_TIMEOUT
/*
 * Document-method: Curses.timeout=
 * call-seq: timeout=(delay)
 *
 * Sets block and non-blocking reads for the window.
 * - If delay is negative, blocking read is used (i.e., waits indefinitely for input).
 * - If delay is zero, then non-blocking read is used (i.e., read returns ERR if no input is waiting).
 * - If delay is positive, then read blocks for delay milliseconds, and returns ERR if there is still no input.
 *
 */
static VALUE
curses_timeout(VALUE obj, VALUE delay)
{
    curses_stdscr();
    timeout(NUM2INT(delay));
    return Qnil;
}
#else
#define curses_timeout rb_f_notimplement
#endif

#ifdef HAVE_DEF_PROG_MODE
/*
 * Document-method: Curses.def_prog_mode
 *
 * Save the current terminal modes as the "program"
 * state for use by the Curses.reset_prog_mode
 *
 * This is done automatically by Curses.init_screen
 */
static VALUE
curses_def_prog_mode(VALUE obj)
{
    curses_stdscr();
    return def_prog_mode() == OK ? Qtrue : Qfalse;
}
#else
#define curses_def_prog_mode rb_f_notimplement
#endif

#ifdef HAVE_RESET_PROG_MODE
/*
 * Document-method: Curses.reset_prog_mode
 *
 * Reset the current terminal modes to the saved state
 * by the Curses.def_prog_mode
 *
 * This is done automatically by Curses.close_screen
 */
static VALUE
curses_reset_prog_mode(VALUE obj)
{
    curses_stdscr();
    return reset_prog_mode() == OK ? Qtrue : Qfalse;
}
#else
#define curses_reset_prog_mode rb_f_notimplement
#endif

/*-------------------------- class Window --------------------------*/

/* returns a Curses::Window object */
static VALUE
window_s_allocate(VALUE class)
{
    struct windata *winp;

    return TypedData_Make_Struct(class, struct windata, &windata_type, winp);
}

/*
 * Document-method: Curses::Window.new
 * call-seq: new(height, width, top, left)
 *
 * Contruct a new Curses::Window with constraints of
 * +height+ lines, +width+ columns, begin at +top+ line, and begin +left+ most column.
 *
 * A new window using full screen is called as
 * 	Curses::Window.new(0,0,0,0)
 *
 */
static VALUE
window_initialize(VALUE obj, VALUE h, VALUE w, VALUE top, VALUE left)
{
    struct windata *winp;
    WINDOW *window;

    rb_secure(4);
    curses_init_screen();
    TypedData_Get_Struct(obj, struct windata, &windata_type, winp);
    if (winp->window) delwin(winp->window);
    window = newwin(NUM2INT(h), NUM2INT(w), NUM2INT(top), NUM2INT(left));
    wclear(window);
    winp->window = window;

    return obj;
}

/*
 * Document-method: Curses::Window.subwin
 * call-seq: subwin(height, width, top, left)
 *
 * Contruct a new subwindow with constraints of
 * +height+ lines, +width+ columns, begin at +top+ line, and begin +left+ most column.
 *
 */
static VALUE
window_subwin(VALUE obj, VALUE height, VALUE width, VALUE top, VALUE left)
{
    struct windata *winp;
    WINDOW *window;
    VALUE win;
    int h, w, t, l;

    h = NUM2INT(height);
    w = NUM2INT(width);
    t = NUM2INT(top);
    l = NUM2INT(left);
    GetWINDOW(obj, winp);
    window = subwin(winp->window, h, w, t, l);
    win = prep_window(rb_obj_class(obj), window);

    return win;
}

/*
 * Document-method: Curses::Window.close
 *
 * Deletes the window, and frees the memory
 */
static VALUE
window_close(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    delwin(winp->window);
    winp->window = 0;

    return Qnil;
}

/*
 * Document-method: Curses::Window.clear
 *
 * Clear the window.
 */
static VALUE
window_clear(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    wclear(winp->window);

    return Qnil;
}

/*
 * Document-method: Curses::Window.clrtoeol
 *
 * Clear the window to the end of line, that the cursor is currently on.
 */
static VALUE
window_clrtoeol(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    wclrtoeol(winp->window);

    return Qnil;
}

/*
 * Document-method: Curses::Window.refresh
 *
 * Refreshes the windows and lines.
 *
 */
static VALUE
window_refresh(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    wrefresh(winp->window);

    return Qnil;
}

/*
 * Document-method: Curses::Window.noutrefresh
 *
 * Refreshes the windows and lines.
 *
 * Curses::Window.noutrefresh allows multiple updates with
 * more efficiency than Curses::Window.refresh alone.
 */
static VALUE
window_noutrefresh(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
#ifdef HAVE_DOUPDATE
    wnoutrefresh(winp->window);
#else
    wrefresh(winp->window);
#endif

    return Qnil;
}

/*
 * Document-method: Curses::Window.move
 * call-seq: move(y,x)
 *
 * Moves the window so that the upper left-hand corner is at position (+y+, +x+)
 */
static VALUE
window_move(VALUE obj, VALUE y, VALUE x)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    mvwin(winp->window, NUM2INT(y), NUM2INT(x));

    return Qnil;
}

/*
 * Document-method: Curses::Window.setpos
 * call-seq: setpos(y, x)
 *
 * A setter for the position of the cursor
 * in the current window,
 * using coordinates +x+ and +y+
 *
 */
static VALUE
window_setpos(VALUE obj, VALUE y, VALUE x)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    wmove(winp->window, NUM2INT(y), NUM2INT(x));
    return Qnil;
}

/*
 * Document-method: Curses::Window.cury
 *
 * A getter for the current line (Y coord) of the window
 */
static VALUE
window_cury(VALUE obj)
{
    struct windata *winp;
    int x, y;

    GetWINDOW(obj, winp);
    getyx(winp->window, y, x);
    return INT2FIX(y);
}

/*
 * Document-method: Curses::Window.curx
 *
 * A getter for the current column (X coord) of the window
 */
static VALUE
window_curx(VALUE obj)
{
    struct windata *winp;
    int x, y;

    GetWINDOW(obj, winp);
    getyx(winp->window, y, x);
    return INT2FIX(x);
}

/*
 * Document-method: Curses::Window.maxy
 *
 * A getter for the maximum lines for the window
 */
static VALUE
window_maxy(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
#if defined(getmaxy)
    return INT2FIX(getmaxy(winp->window));
#elif defined(getmaxyx)
    {
	int x, y;
	getmaxyx(winp->window, y, x);
	return INT2FIX(y);
    }
#else
    return INT2FIX(winp->window->_maxy+1);
#endif
}

/*
 * Document-method: Curses::Window.maxx
 *
 * A getter for the maximum columns for the window
 */
static VALUE
window_maxx(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
#if defined(getmaxx)
    return INT2FIX(getmaxx(winp->window));
#elif defined(getmaxyx)
    {
	int x, y;
	getmaxyx(winp->window, y, x);
	return INT2FIX(x);
    }
#else
    return INT2FIX(winp->window->_maxx+1);
#endif
}

/*
 * Document-method: Curses::Window.begy
 *
 * A getter for the beginning line (Y coord) of the window
 */
static VALUE
window_begy(VALUE obj)
{
    struct windata *winp;
    int x, y;

    GetWINDOW(obj, winp);
#ifdef getbegyx
    getbegyx(winp->window, y, x);
    return INT2FIX(y);
#else
    return INT2FIX(winp->window->_begy);
#endif
}

/*
 * Document-method: Curses::Window.begx
 *
 * A getter for the beginning column (X coord) of the window
 */
static VALUE
window_begx(VALUE obj)
{
    struct windata *winp;
    int x, y;

    GetWINDOW(obj, winp);
#ifdef getbegyx
    getbegyx(winp->window, y, x);
    return INT2FIX(x);
#else
    return INT2FIX(winp->window->_begx);
#endif
}

/*
 * Document-method: Curses::Window.box
 * call-seq: box(vert, hor)
 *
 * set the characters to frame the window in.
 * The vertical +vert+ and horizontal +hor+ character.
 *
 * 	win = Curses::Window.new(5,5,5,5)
 * 	win.box(?|, ?-)
 *
 */
static VALUE
window_box(int argc, VALUE *argv, VALUE self)
{
    struct windata *winp;
    VALUE vert, hor, corn;

    rb_scan_args(argc, argv, "21", &vert, &hor, &corn);

    GetWINDOW(self, winp);
    box(winp->window, NUM2CH(vert), NUM2CH(hor));

    if (!NIL_P(corn)) {
	int cur_x, cur_y, x, y;
	chtype c;

	c = NUM2CH(corn);
	getyx(winp->window, cur_y, cur_x);
	x = NUM2INT(window_maxx(self)) - 1;
	y = NUM2INT(window_maxy(self)) - 1;
	wmove(winp->window, 0, 0);
	waddch(winp->window, c);
	wmove(winp->window, y, 0);
	waddch(winp->window, c);
	wmove(winp->window, y, x);
	waddch(winp->window, c);
	wmove(winp->window, 0, x);
	waddch(winp->window, c);
	wmove(winp->window, cur_y, cur_x);
    }

    return Qnil;
}

/*
 * Document-method: Curses::Window.standout
 *
 * Enables the best highlighting mode of the terminal.
 *
 * This is equivalent to Curses::Window.attron(A_STANDOUT)
 *
 * see also Curses::Window.attrset
 */
static VALUE
window_standout(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    wstandout(winp->window);
    return Qnil;
}

/*
 * Document-method: Curses::Window.standend
 *
 * Enables the Normal display (no highlight)
 *
 * This is equivalent to Curses::Window.attron(A_NORMAL)
 *
 * see also Curses::Window.attrset
 */
static VALUE
window_standend(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    wstandend(winp->window);
    return Qnil;
}

/*
 * Document-method: Curses::Window.inch
 *
 * Returns the character at the current position of the window.
 */
static VALUE
window_inch(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    return CH2FIX(winch(winp->window));
}

/*
 * Document-method: Curses::Window.addch
 * call-seq: addch(ch)
 *
 * Add a character +ch+, with attributes, to the window, then advance the cursor.
 *
 * see also the system manual for curs_addch(3)
 */
static VALUE
window_addch(VALUE obj, VALUE ch)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    waddch(winp->window, NUM2CH(ch));

    return Qnil;
}

/*
 * Document-method: Curses::Window.insch
 * call-seq: insch(ch)
 *
 * Insert a character +ch+, before the cursor, in the current window
 *
 */
static VALUE
window_insch(VALUE obj, VALUE ch)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    winsch(winp->window, NUM2CH(ch));

    return Qnil;
}

/*
 * Document-method: Curses::Window.addstr
 * call-seq: addstr(str)
 *
 * add a string of characters +str+, to the window and advance cursor
 *
 */
static VALUE
window_addstr(VALUE obj, VALUE str)
{
    if (!NIL_P(str)) {
	struct windata *winp;

	StringValue(str);
	str = rb_str_export_locale(str);
	GetWINDOW(obj, winp);
	waddstr(winp->window, StringValueCStr(str));
    }
    return Qnil;
}

/*
 * Document-method: Curses::Window.<<
 *
 * call-seq:
 *   <<(str)
 *
 * Add String +str+ to the current string.
 *
 * See also Curses::Window.addstr
 */
static VALUE
window_addstr2(VALUE obj, VALUE str)
{
    window_addstr(obj, str);
    return obj;
}

struct wgetch_arg {
    WINDOW *win;
    int c;
};

static VALUE
wgetch_func(void *_arg)
{
    struct wgetch_arg *arg = (struct wgetch_arg *)_arg;
    arg->c = wgetch(arg->win);
    return Qnil;
}

/*
 * Document-method: Curses::Window.getch
 *
 * Read and returns a character from the window.
 *
 * See Curses::Key to all the function KEY_* available
 *
 */
static VALUE
window_getch(VALUE obj)
{
    struct windata *winp;
    struct wgetch_arg arg;
    int c;

    GetWINDOW(obj, winp);
    arg.win = winp->window;
    rb_thread_blocking_region(wgetch_func, (void *)&arg, RUBY_UBF_IO, 0);
    c = arg.c;
    if (c == EOF) return Qnil;
    if (rb_isprint(c)) {
	char ch = (char)c;

	return rb_locale_str_new(&ch, 1);
    }
    return UINT2NUM(c);
}

struct wgetstr_arg {
    WINDOW *win;
    char rtn[GETSTR_BUF_SIZE];
};

static VALUE
wgetstr_func(void *_arg)
{
    struct wgetstr_arg *arg = (struct wgetstr_arg *)_arg;
#if defined(HAVE_WGETNSTR)
    wgetnstr(arg->win, arg->rtn, GETSTR_BUF_SIZE-1);
#else
    wgetstr(arg->win, arg->rtn);
#endif
    return Qnil;
}

/*
 * Document-method: Curses::Window.getstr
 *
 * This is equivalent to a series f Curses::Window.getch calls
 *
 */
static VALUE
window_getstr(VALUE obj)
{
    struct windata *winp;
    struct wgetstr_arg arg;

    GetWINDOW(obj, winp);
    arg.win = winp->window;
    rb_thread_blocking_region(wgetstr_func, (void *)&arg, RUBY_UBF_IO, 0);
    return rb_locale_str_new_cstr(arg.rtn);
}

/*
 * Document-method: Curses::Window.delch
 *
 * Delete the character under the cursor
 *
 */
static VALUE
window_delch(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    wdelch(winp->window);
    return Qnil;
}

/*
 * Document-method: Curses::Window.deleteln
 *
 * Delete the line under the cursor.
 *
 */
static VALUE
window_deleteln(VALUE obj)
{
#if defined(HAVE_WDELETELN) || defined(wdeleteln)
    struct windata *winp;

    GetWINDOW(obj, winp);
    wdeleteln(winp->window);
#endif
    return Qnil;
}

/*
 * Document-method: Curses::Window.insertln
 *
 * Inserts a line above the cursor, and the bottom line is lost
 *
 */
static VALUE
window_insertln(VALUE obj)
{
#if defined(HAVE_WINSERTLN) || defined(winsertln)
    struct windata *winp;

    GetWINDOW(obj, winp);
    winsertln(winp->window);
#endif
    return Qnil;
}

/*
 * Document-method: Curses::Window.scrollok
 * call-seq: scrollok(bool)
 *
 * Controls what happens when the cursor of a window
 * is moved off the edge of the window or scrolling region,
 * either as a result of a newline action on the bottom line,
 * or typing the last character of the last line.
 *
 * If disabled, (+bool+ is false), the cursor is left on the bottom line.
 *
 * If enabled, (+bool+ is true), the window is scrolled up one line
 * (Note that to get the physical scrolling effect on the terminal,
 * it is also necessary to call Curses::Window.idlok)
 */
static VALUE
window_scrollok(VALUE obj, VALUE bf)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    scrollok(winp->window, RTEST(bf) ? TRUE : FALSE);
    return Qnil;
}

/*
 * Document-method: Curses::Window.idlok
 * call-seq: idlok(bool)
 *
 * If +bool+ is +true+ curses considers using the hardware insert/delete
 * line feature of terminals so equipped.
 *
 * If +bool+ is +false+, disables use of line insertion and deletion.
 * This option should be enabled only if the application needs insert/delete
 * line, for example, for a screen editor.
 *
 * It is disabled by default because insert/delete line tends to be visually
 * annoying when used in applications where it is not really needed.
 * If insert/delete line cannot be used, curses redraws the changed portions of all lines.
 *
 */
static VALUE
window_idlok(VALUE obj, VALUE bf)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    idlok(winp->window, RTEST(bf) ? TRUE : FALSE);
    return Qnil;
}

/*
 * Document-method: Curses::Window.setscrreg
 * call-seq:
 *   setscrreg(top, bottom)
 *
 * Set a software scrolling region in a window.
 * +top+ and +bottom+ are lines numbers of the margin.
 *
 * If this option and Curses::Window.scrollok are enabled, an attempt to move
 * off the bottom margin line causes all lines in the scrolling region to
 * scroll one line in the direction of the first line.  Only the text of the
 * window is scrolled.
 *
 */
static VALUE
window_setscrreg(VALUE obj, VALUE top, VALUE bottom)
{
#ifdef HAVE_WSETSCRREG
    struct windata *winp;
    int res;

    GetWINDOW(obj, winp);
    res = wsetscrreg(winp->window, NUM2INT(top), NUM2INT(bottom));
    /* may have to raise exception on ERR */
    return (res == OK) ? Qtrue : Qfalse;
#else
    return Qfalse;
#endif
}

#if defined(USE_COLOR) && defined(HAVE_WCOLOR_SET)
/*
 * Document-method: Curses::Window.color_set
 * call-seq: color_set(col)
 *
 * Sets the current color of the given window to the
 * foreground/background combination described by the Fixnum +col+.
 */
static VALUE
window_color_set(VALUE obj, VALUE col)
{
    struct windata *winp;
    int res;

    GetWINDOW(obj, winp);
    res = wcolor_set(winp->window, NUM2INT(col), NULL);
    return (res == OK) ? Qtrue : Qfalse;
}
#endif /* defined(USE_COLOR) && defined(HAVE_WCOLOR_SET) */

/*
 * Document-method: Curses::Window.scroll
 *
 * Scrolls the current window up one line.
 */
static VALUE
window_scroll(VALUE obj)
{
    struct windata *winp;

    GetWINDOW(obj, winp);
    /* may have to raise exception on ERR */
    return (scroll(winp->window) == OK) ? Qtrue : Qfalse;
}

/*
 * Document-method: Curses::Window.scrl
 * call-seq: scrl(num)
 *
 * Scrolls the current window Fixnum +num+ lines.
 * The current cursor position is not changed.
 *
 * For positive +num+, it scrolls up.
 *
 * For negative +num+, it scrolls down.
 *
 */
static VALUE
window_scrl(VALUE obj, VALUE n)
{
#ifdef HAVE_WSCRL
    struct windata *winp;

    GetWINDOW(obj, winp);
    /* may have to raise exception on ERR */
    return (wscrl(winp->window,NUM2INT(n)) == OK) ? Qtrue : Qfalse;
#else
    return Qfalse;
#endif
}

/*
 * Document-method: Curses::Window.attroff
 * call-seq: attroff(attrs)
 *
 * Turns on the named attributes +attrs+ without affecting any others.
 *
 * See also Curses::Window.attrset
 */
static VALUE
window_attroff(VALUE obj, VALUE attrs)
{
#ifdef HAVE_WATTROFF
    struct windata *winp;

    GetWINDOW(obj,winp);
    return INT2FIX(wattroff(winp->window,NUM2INT(attrs)));
#else
    return Qtrue;
#endif
}

/*
 * Document-method: Curses::Window.attron
 * call-seq: attron(attrs)
 *
 * Turns off the named attributes +attrs+
 * without turning any other attributes on or off.
 *
 * See also Curses::Window.attrset
 */
static VALUE
window_attron(VALUE obj, VALUE attrs)
{
#ifdef HAVE_WATTRON
    struct windata *winp;
    VALUE val;

    GetWINDOW(obj,winp);
    val = INT2FIX(wattron(winp->window,NUM2INT(attrs)));
    if (rb_block_given_p()) {
	rb_yield(val);
	wattroff(winp->window,NUM2INT(attrs));
	return val;
    }
    else{
	return val;
    }
#else
    return Qtrue;
#endif
}

/*
 * Document-method: Curses::Window.attrset
 * call-seq: attrset(attrs)
 *
 * Sets the current attributes of the given window to +attrs+.
 *
 * The following video attributes, defined in <curses.h>, can
 * be passed to the routines Curses::Window.attron, Curses::Window.attroff,
 * and Curses::Window.attrset, or OR'd with the characters passed to addch.
 *   A_NORMAL        Normal display (no highlight)
 *   A_STANDOUT      Best highlighting mode of the terminal.
 *   A_UNDERLINE     Underlining
 *   A_REVERSE       Reverse video
 *   A_BLINK         Blinking
 *   A_DIM           Half bright
 *   A_BOLD          Extra bright or bold
 *   A_PROTECT       Protected mode
 *   A_INVIS         Invisible or blank mode
 *   A_ALTCHARSET    Alternate character set
 *   A_CHARTEXT      Bit-mask to extract a character
 *   COLOR_PAIR(n)   Color-pair number n
 *
 * TODO: provide some examples here.
 *
 * see also system manual curs_attr(3)
 */
static VALUE
window_attrset(VALUE obj, VALUE attrs)
{
#ifdef HAVE_WATTRSET
    struct windata *winp;

    GetWINDOW(obj,winp);
    return INT2FIX(wattrset(winp->window,NUM2INT(attrs)));
#else
    return Qtrue;
#endif
}

/*
 * Document-method: Curses::Window.bkgdset
 * call-seq: bkgdset(ch)
 *
 * Manipulate the background of the current window
 * with character Integer +ch+
 *
 * see also Curses.bkgdset
 */
static VALUE
window_bkgdset(VALUE obj, VALUE ch)
{
#ifdef HAVE_WBKGDSET
    struct windata *winp;

    GetWINDOW(obj,winp);
    wbkgdset(winp->window, NUM2CH(ch));
#endif
    return Qnil;
}

/*
 * Document-method: Curses::Window.bkgd
 * call-seq: bkgd(ch)
 *
 * Set the background of the current window
 * and apply character Integer +ch+ to every character.
 *
 * see also Curses.bkgd
 */
static VALUE
window_bkgd(VALUE obj, VALUE ch)
{
#ifdef HAVE_WBKGD
    struct windata *winp;

    GetWINDOW(obj,winp);
    return (wbkgd(winp->window, NUM2CH(ch)) == OK) ? Qtrue : Qfalse;
#else
    return Qfalse;
#endif
}

/*
 * Document-method: Curses::Window.getbkgd
 *
 * Returns an Interer (+ch+) for the character property in the current window.
 */
static VALUE
window_getbkgd(VALUE obj)
{
#ifdef HAVE_WGETBKGD
    chtype c;
    struct windata *winp;

    GetWINDOW(obj,winp);
    return (c = getbkgd(winp->window) != ERR) ? CH2FIX(c) : Qnil;
#else
    return Qnil;
#endif
}

/*
 * Document-method: Curses::Window.resize
 * call-seq: resize(lines, cols)
 *
 * Resize the current window to Fixnum +lines+ and Fixnum +cols+
 *
 */
static VALUE
window_resize(VALUE obj, VALUE lin, VALUE col)
{
#if defined(HAVE_WRESIZE)
    struct windata *winp;

    GetWINDOW(obj,winp);
    return wresize(winp->window, NUM2INT(lin), NUM2INT(col)) == OK ? Qtrue : Qfalse;
#else
    return Qnil;
#endif
}


#ifdef HAVE_KEYPAD
/*
 * Document-method: Curses::Window.keypad=
 * call-seq:
 *   keypad=(bool)
 *
 * See Curses::Window.keypad
 */

/*
 * Document-method: Curses::Window.keypad
 * call-seq:
 *   keypad(bool)
 *
 * Enables the keypad of the user's terminal.
 *
 * If enabled (+bool+ is +true+), the user can press a function key
 * (such as an arrow key) and wgetch returns a single value representing
 * the function key, as in KEY_LEFT.  If disabled (+bool+ is +false+),
 * curses does not treat function keys specially and the program has to
 * interpret the escape sequences itself.  If the keypad in the terminal
 * can be turned on (made to transmit) and off (made to work locally),
 * turning on this option causes the terminal keypad to be turned on when
 * Curses::Window.getch is called.
 *
 * The default value for keypad is false.
 *
 */
static VALUE
window_keypad(VALUE obj, VALUE val)
{
    struct windata *winp;

    GetWINDOW(obj,winp);
    /* keypad() of NetBSD's libcurses returns no value */
#if defined(__NetBSD__) && !defined(NCURSES_VERSION)
    keypad(winp->window,(RTEST(val) ? TRUE : FALSE));
    return Qnil;
#else
    /* may have to raise exception on ERR */
    return (keypad(winp->window,RTEST(val) ? TRUE : FALSE)) == OK ?
	Qtrue : Qfalse;
#endif
}
#else
#define window_keypad rb_f_notimplement
#endif

#ifdef HAVE_NODELAY
/*
 * Document-method: Curses::Window.nodelay
 * call-seq: nodelay(bool)
 *
 * Causes Curses::Window.getch to be a non-blocking call.  If no input is ready, getch returns ERR.
 *
 * If disabled (+bool+ is +false+), Curses::Window.getch waits until a key is pressed.
 *
 */
static VALUE
window_nodelay(VALUE obj, VALUE val)
{
    struct windata *winp;
    GetWINDOW(obj,winp);

    /* nodelay() of NetBSD's libcurses returns no value */
#if defined(__NetBSD__) && !defined(NCURSES_VERSION)
    nodelay(winp->window, RTEST(val) ? TRUE : FALSE);
    return Qnil;
#else
    return nodelay(winp->window,RTEST(val) ? TRUE : FALSE) == OK ? Qtrue : Qfalse;
#endif
}
#else
#define window_nodelay rb_f_notimplement
#endif

#ifdef HAVE_WTIMEOUT
/*
 * Document-method: Curses::Window.timeout=
 * call-seq: timeout=(delay)
 *
 * Sets block and non-blocking reads for the window.
 * - If delay is negative, blocking read is used (i.e., waits indefinitely for input).
 * - If delay is zero, then non-blocking read is used (i.e., read returns ERR if no input is waiting).
 * - If delay is positive, then read blocks for delay milliseconds, and returns ERR if there is still no input.
 *
 */
static VALUE
window_timeout(VALUE obj, VALUE delay)
{
    struct windata *winp;
    GetWINDOW(obj,winp);

    wtimeout(winp->window,NUM2INT(delay));
    return Qnil;
}
#else
#define window_timeout rb_f_notimplement
#endif

/*--------------------------- class Pad ----------------------------*/

#ifdef HAVE_NEWPAD
/*
 * Document-method: Curses::Pad.new
 *
 * call-seq:
 *   new(height, width)
 *
 * Contruct a new Curses::Pad with constraints of +height+ lines, +width+
 * columns
 *
 */
static VALUE
pad_initialize(VALUE obj, VALUE h, VALUE w)
{
    struct windata *padp;
    WINDOW *window;

    rb_secure(4);
    curses_init_screen();
    TypedData_Get_Struct(obj, struct windata, &windata_type, padp);
    if (padp->window) delwin(padp->window);
    window = newpad(NUM2INT(h), NUM2INT(w));
    wclear(window);
    padp->window = window;

    return obj;
}

#if 1
#define pad_subpad window_subwin
#else
/*
 * Document-method: Curses::Pad.subpad
 * call-seq:
 *   subpad(height, width, begin_x, begin_y)
 *
 * Contruct a new subpad with constraints of +height+ lines, +width+ columns,
 * begin at +begin_x+ line, and +begin_y+ columns on the pad.
 *
 */
static VALUE
pad_subpad(VALUE obj, VALUE height, VALUE width, VALUE begin_x, VALUE begin_y)
{
    struct windata *padp;
    WINDOW *subpad;
    VALUE pad;
    int h, w, x, y;

    h = NUM2INT(height);
    w = NUM2INT(width);
    x = NUM2INT(begin_x);
    y = NUM2INT(begin_y);
    GetWINDOW(obj, padp);
    subpad = subwin(padp->window, h, w, x, y);
    pad = prep_window(rb_obj_class(obj), subpad);

    return pad;
}
#endif

/*
 * Document-method: Curses::Pad.refresh
 *
 * call-seq:
 *   pad.refresh(pad_minrow, pad_mincol, screen_minrow, screen_mincol, screen_maxrow, screen_maxcol)
 *
 * Refreshes the pad.  +pad_minrow+ and pad_mincol+ define the upper-left
 * corner of the rectangle to be displayed.  +screen_minrow+, +screen_mincol+,
 * +screen_maxrow+, +screen_maxcol+ define the edges of the rectangle to be
 * displayed on the screen.
 *
 */
static VALUE
pad_refresh(VALUE obj, VALUE pminrow, VALUE pmincol, VALUE sminrow,
	    VALUE smincol, VALUE smaxrow, VALUE smaxcol)
{
    struct windata *padp;
    int pmr, pmc, smr, smc, sxr, sxc;

    pmr = NUM2INT(pminrow);
    pmc = NUM2INT(pmincol);
    smr = NUM2INT(sminrow);
    smc = NUM2INT(smincol);
    sxr = NUM2INT(smaxrow);
    sxc = NUM2INT(smaxcol);

    GetWINDOW(obj, padp);
    prefresh(padp->window, pmr, pmc, smr, smc, sxr, sxc);

    return Qnil;
}

/*
 * Document-method: Curses::Pad.noutrefresh
 *
 * call-seq:
 *   pad.noutrefresh(pad_minrow, pad_mincol, screen_minrow, screen_mincol, screen_maxrow, screen_maxcol)
 *
 * Refreshes the pad.  +pad_minrow+ and pad_mincol+ define the upper-left
 * corner of the rectangle to be displayed.  +screen_minrow+, +screen_mincol+,
 * +screen_maxrow+, +screen_maxcol+ define the edges of the rectangle to be
 * displayed on the screen.
 *
 */
static VALUE
pad_noutrefresh(VALUE obj, VALUE pminrow, VALUE pmincol, VALUE sminrow,
		VALUE smincol, VALUE smaxrow, VALUE smaxcol)
{
    struct windata *padp;
    int pmr, pmc, smr, smc, sxr, sxc;

    pmr = NUM2INT(pminrow);
    pmc = NUM2INT(pmincol);
    smr = NUM2INT(sminrow);
    smc = NUM2INT(smincol);
    sxr = NUM2INT(smaxrow);
    sxc = NUM2INT(smaxcol);

    GetWINDOW(obj, padp);
#ifdef HAVE_DOUPDATE
    pnoutrefresh(padp->window, pmr, pmc, smr, smc, sxr, sxc);
#else
    prefresh(padp->window, pmr, pmc, smr, smc, sxr, sxc);
#endif

    return Qnil;
}
#endif /* HAVE_NEWPAD */

/*------------------------- Initialization -------------------------*/

/*
 * Document-module: Curses
 *
 * == Description
 * An implementation of the CRT screen handling and optimization library.
 *
 * == Structures and such
 *
 * === Classes
 *
 * * Curses::Window - class with the means to draw a window or box
 * * Curses::MouseEvent - class for collecting mouse events
 *
 * === Modules
 *
 * Curses:: The curses implementation
 * Curses::Key:: Collection of constants for keypress events
 *
 * == Examples
 *
 * * hello.rb
 *     :include: hello.rb
 *
 *
 * * rain.rb
 *     :include: rain.rb
 *
 *
 */
void
Init_curses(void)
{
    mCurses    = rb_define_module("Curses");

    /*
     * Document-module: Curses::Key
     *
     *
     * a container for the KEY_* values.
     *
     * See also system manual for getch(3)
     *
     */
    mKey       = rb_define_module_under(mCurses, "Key");

    rb_gc_register_address(&rb_stdscr);

#ifdef USE_MOUSE
    /*
     * Document-class: Curses::MouseEvent
     *
     * == Description
     *
     * Curses::MouseEvent
     *
     * == Example
     *
     * * mouse.rb
     *     :include: mouse.rb
     *
     */
    cMouseEvent = rb_define_class_under(mCurses,"MouseEvent",rb_cObject);
    rb_undef_method(CLASS_OF(cMouseEvent),"new");
    rb_define_method(cMouseEvent, "eid", curs_mouse_id, 0);
    rb_define_method(cMouseEvent, "x", curs_mouse_x, 0);
    rb_define_method(cMouseEvent, "y", curs_mouse_y, 0);
    rb_define_method(cMouseEvent, "z", curs_mouse_z, 0);
    rb_define_method(cMouseEvent, "bstate", curs_mouse_bstate, 0);
#endif /* USE_MOUSE */

    rb_define_module_function(mCurses, "ESCDELAY=", curses_escdelay_set, 1);
    rb_define_module_function(mCurses, "ESCDELAY", curses_escdelay_get, 0);
    rb_define_module_function(mCurses, "TABSIZE", curses_tabsize_get, 0);
    rb_define_module_function(mCurses, "TABSIZE=", curses_tabsize_set, 1);

    rb_define_module_function(mCurses, "use_default_colors", curses_use_default_colors, 0);
    rb_define_module_function(mCurses, "init_screen", curses_init_screen, 0);
    rb_define_module_function(mCurses, "close_screen", curses_close_screen, 0);
    rb_define_module_function(mCurses, "closed?", curses_closed, 0);
    rb_define_module_function(mCurses, "stdscr", curses_stdscr, 0);
    rb_define_module_function(mCurses, "refresh", curses_refresh, 0);
    rb_define_module_function(mCurses, "doupdate", curses_doupdate, 0);
    rb_define_module_function(mCurses, "clear", curses_clear, 0);
    rb_define_module_function(mCurses, "clrtoeol", curses_clrtoeol, 0);
    rb_define_module_function(mCurses, "echo", curses_echo, 0);
    rb_define_module_function(mCurses, "noecho", curses_noecho, 0);
    rb_define_module_function(mCurses, "raw", curses_raw, 0);
    rb_define_module_function(mCurses, "noraw", curses_noraw, 0);
    rb_define_module_function(mCurses, "cbreak", curses_cbreak, 0);
    rb_define_module_function(mCurses, "nocbreak", curses_nocbreak, 0);
    rb_define_module_function(mCurses, "crmode", curses_nocbreak, 0);
    rb_define_module_function(mCurses, "nocrmode", curses_nocbreak, 0);
    rb_define_module_function(mCurses, "nl", curses_nl, 0);
    rb_define_module_function(mCurses, "nonl", curses_nonl, 0);
    rb_define_module_function(mCurses, "beep", curses_beep, 0);
    rb_define_module_function(mCurses, "flash", curses_flash, 0);
    rb_define_module_function(mCurses, "ungetch", curses_ungetch, 1);
    rb_define_module_function(mCurses, "setpos", curses_setpos, 2);
    rb_define_module_function(mCurses, "standout", curses_standout, 0);
    rb_define_module_function(mCurses, "standend", curses_standend, 0);
    rb_define_module_function(mCurses, "inch", curses_inch, 0);
    rb_define_module_function(mCurses, "addch", curses_addch, 1);
    rb_define_module_function(mCurses, "insch", curses_insch, 1);
    rb_define_module_function(mCurses, "addstr", curses_addstr, 1);
    rb_define_module_function(mCurses, "getch", curses_getch, 0);
    rb_define_module_function(mCurses, "getstr", curses_getstr, 0);
    rb_define_module_function(mCurses, "delch", curses_delch, 0);
    rb_define_module_function(mCurses, "deleteln", curses_deleteln, 0);
    rb_define_module_function(mCurses, "insertln", curses_insertln, 0);
    rb_define_module_function(mCurses, "keyname", curses_keyname, 1);
    rb_define_module_function(mCurses, "lines", curses_lines, 0);
    rb_define_module_function(mCurses, "cols", curses_cols, 0);
    rb_define_module_function(mCurses, "curs_set", curses_curs_set, 1);
    rb_define_module_function(mCurses, "scrl", curses_scrl, 1);
    rb_define_module_function(mCurses, "setscrreg", curses_setscrreg, 2);
    rb_define_module_function(mCurses, "attroff", curses_attroff, 1);
    rb_define_module_function(mCurses, "attron", curses_attron, 1);
    rb_define_module_function(mCurses, "attrset", curses_attrset, 1);
    rb_define_module_function(mCurses, "bkgdset", curses_bkgdset, 1);
    rb_define_module_function(mCurses, "bkgd", curses_bkgd, 1);
    rb_define_module_function(mCurses, "resizeterm", curses_resizeterm, 2);
    rb_define_module_function(mCurses, "resize", curses_resizeterm, 2);
#ifdef USE_COLOR
    rb_define_module_function(mCurses, "start_color", curses_start_color, 0);
    rb_define_module_function(mCurses, "init_pair", curses_init_pair, 3);
    rb_define_module_function(mCurses, "init_color", curses_init_color, 4);
    rb_define_module_function(mCurses, "has_colors?", curses_has_colors, 0);
    rb_define_module_function(mCurses, "can_change_color?",
			      curses_can_change_color, 0);
    rb_define_module_function(mCurses, "colors", curses_colors, 0);
    rb_define_module_function(mCurses, "color_content", curses_color_content, 1);
    rb_define_module_function(mCurses, "color_pairs", curses_color_pairs, 0);
    rb_define_module_function(mCurses, "pair_content", curses_pair_content, 1);
    rb_define_module_function(mCurses, "color_pair", curses_color_pair, 1);
    rb_define_module_function(mCurses, "pair_number", curses_pair_number, 1);
#endif /* USE_COLOR */
#ifdef USE_MOUSE
    rb_define_module_function(mCurses, "getmouse", curses_getmouse, 0);
    rb_define_module_function(mCurses, "ungetmouse", curses_ungetmouse, 1);
    rb_define_module_function(mCurses, "mouseinterval", curses_mouseinterval, 1);
    rb_define_module_function(mCurses, "mousemask", curses_mousemask, 1);
#endif /* USE_MOUSE */

    rb_define_module_function(mCurses, "timeout=", curses_timeout, 1);
    rb_define_module_function(mCurses, "def_prog_mode", curses_def_prog_mode, 0);
    rb_define_module_function(mCurses, "reset_prog_mode", curses_reset_prog_mode, 0);

    {
        VALUE version;
#if defined(HAVE_FUNC_CURSES_VERSION)
        /* ncurses and PDcurses */
        version = rb_str_new2(curses_version());
#elif defined(HAVE_VAR_CURSES_VERSION)
        /* SVR4 curses has an undocumented and undeclared variable, curses_version.
         * It contains a string, "SVR4".  */
        RUBY_EXTERN char *curses_version;
        version = rb_sprintf("curses (%s)", curses_version);
#else
        /* BSD curses, perhaps.  NetBSD 5 still use it. */ 
        version = rb_str_new2("curses (unknown)");
#endif
        /*
         * Identifies curses library version.
         *
         * - "ncurses 5.9.20110404"
         * - "PDCurses 3.4 - Public Domain 2008"
         * - "curses (SVR4)" (System V curses)
         * - "curses (unknown)" (The original BSD curses?  NetBSD maybe.)
         *
         */
        rb_define_const(mCurses, "VERSION", version);
    }

    /*
     * Document-class: Curses::Window
     *
     * == Description
     *
     * The means by which to create and manage frames or windows.
     * While there may be more than one window at a time, only one window
     * will receive input.
     *
     * == Usage
     *
     *   require 'curses'
     *
     *   Curses.init_screen()
     *
     *   my_str = "LOOK! PONIES!"
     *   win = Curses::Window.new( 8, (my_str.length + 10),
     *                             (Curses.lines - 8) / 2,
     *                             (Curses.cols - (my_str.length + 10)) / 2 )
     *   win.box("|", "-")
     *   win.setpos(2,3)
     *   win.addstr(my_str)
     *   # or even
     *   win << "\nORLY"
     *   win << "\nYES!! " + my_str
     *   win.refresh
     *   win.getch
     *   win.close
     *
     */
    cWindow = rb_define_class_under(mCurses, "Window", rb_cData);
    rb_define_alloc_func(cWindow, window_s_allocate);
    rb_define_method(cWindow, "initialize", window_initialize, 4);
    rb_define_method(cWindow, "subwin", window_subwin, 4);
    rb_define_method(cWindow, "close", window_close, 0);
    rb_define_method(cWindow, "clear", window_clear, 0);
    rb_define_method(cWindow, "clrtoeol", window_clrtoeol, 0);
    rb_define_method(cWindow, "refresh", window_refresh, 0);
    rb_define_method(cWindow, "noutrefresh", window_noutrefresh, 0);
    rb_define_method(cWindow, "box", window_box, -1);
    rb_define_method(cWindow, "move", window_move, 2);
    rb_define_method(cWindow, "setpos", window_setpos, 2);
#if defined(USE_COLOR) && defined(HAVE_WCOLOR_SET)
    rb_define_method(cWindow, "color_set", window_color_set, 1);
#endif /* USE_COLOR && HAVE_WCOLOR_SET */
    rb_define_method(cWindow, "cury", window_cury, 0);
    rb_define_method(cWindow, "curx", window_curx, 0);
    rb_define_method(cWindow, "maxy", window_maxy, 0);
    rb_define_method(cWindow, "maxx", window_maxx, 0);
    rb_define_method(cWindow, "begy", window_begy, 0);
    rb_define_method(cWindow, "begx", window_begx, 0);
    rb_define_method(cWindow, "standout", window_standout, 0);
    rb_define_method(cWindow, "standend", window_standend, 0);
    rb_define_method(cWindow, "inch", window_inch, 0);
    rb_define_method(cWindow, "addch", window_addch, 1);
    rb_define_method(cWindow, "insch", window_insch, 1);
    rb_define_method(cWindow, "addstr", window_addstr, 1);
    rb_define_method(cWindow, "<<", window_addstr2, 1);
    rb_define_method(cWindow, "getch", window_getch, 0);
    rb_define_method(cWindow, "getstr", window_getstr, 0);
    rb_define_method(cWindow, "delch", window_delch, 0);
    rb_define_method(cWindow, "deleteln", window_deleteln, 0);
    rb_define_method(cWindow, "insertln", window_insertln, 0);
    rb_define_method(cWindow, "scroll", window_scroll, 0);
    rb_define_method(cWindow, "scrollok", window_scrollok, 1);
    rb_define_method(cWindow, "idlok", window_idlok, 1);
    rb_define_method(cWindow, "setscrreg", window_setscrreg, 2);
    rb_define_method(cWindow, "scrl", window_scrl, 1);
    rb_define_method(cWindow, "resize", window_resize, 2);
    rb_define_method(cWindow, "keypad", window_keypad, 1);
    rb_define_method(cWindow, "keypad=", window_keypad, 1);

    rb_define_method(cWindow, "attroff", window_attroff, 1);
    rb_define_method(cWindow, "attron", window_attron, 1);
    rb_define_method(cWindow, "attrset", window_attrset, 1);
    rb_define_method(cWindow, "bkgdset", window_bkgdset, 1);
    rb_define_method(cWindow, "bkgd", window_bkgd, 1);
    rb_define_method(cWindow, "getbkgd", window_getbkgd, 0);

    rb_define_method(cWindow, "nodelay=", window_nodelay, 1);
    rb_define_method(cWindow, "timeout=", window_timeout, 1);

#ifdef HAVE_NEWPAD
    /*
     * Document-class: Curses::Pad
     *
     * == Description
     *
     * A Pad is like a Window but allows for scrolling of contents that cannot
     * fit on the screen.  Pads do not refresh automatically, use Pad#refresh
     * or Pad#noutrefresh instead.
     *
     */
    cPad = rb_define_class_under(mCurses, "Pad", cWindow);
    /* inherits alloc_func from cWindow */
    rb_define_method(cPad, "initialize", pad_initialize, 2);
    rb_define_method(cPad, "subpad", pad_subpad, 4);
    rb_define_method(cPad, "refresh", pad_refresh, 6);
    rb_define_method(cPad, "noutrefresh", pad_noutrefresh, 6);
    rb_undef_method(cPad, "subwin");
#endif

#define rb_curses_define_const(c) rb_define_const(mCurses,#c,UINT2NUM(c))

#ifdef USE_COLOR
    /* Document-const: A_ATTRIBUTES
     *
     * Character attribute mask:
     * Bit-mask to extract attributes
     *
     * See Curses.inch or Curses::Window.inch
     */
    rb_curses_define_const(A_ATTRIBUTES);
#ifdef A_NORMAL
    /* Document-const: A_NORMAL
     *
     * Attribute mask:
     * Normal display (no highlight)
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_NORMAL);
#endif
    /* Document-const: A_STANDOUT
     *
     * Attribute mask:
     * Best highlighting mode of the terminal.
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_STANDOUT);
    /* Document-const: A_UNDERLINE
     *
     * Attribute mask:
     * Underlining
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_UNDERLINE);
    /* Document-const: A_REVERSE
     *
     * Attribute mask:
     * Reverse video
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_REVERSE);
    /* Document-const: A_BLINK
     *
     * Attribute mask:
     * Blinking
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_BLINK);
    /* Document-const: A_DIM
     *
     * Attribute mask:
     * Half bright
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_DIM);
    /* Document-const: A_BOLD
     *
     * Attribute mask:
     * Extra bright or bold
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_BOLD);
    /* Document-const: A_PROTECT
     *
     * Attribute mask:
     * Protected mode
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_PROTECT);
#ifdef A_INVIS /* for NetBSD */
    /* Document-const: A_INVIS
     *
     * Attribute mask:
     * Invisible or blank mode
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_INVIS);
#endif
    /* Document-const: A_ALTCHARSET
     *
     * Attribute mask:
     * Alternate character set
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_ALTCHARSET);
    /* Document-const: A_CHARTEXT
     *
     * Attribute mask:
     * Bit-mask to extract a character
     *
     * See Curses.attrset
     */
    rb_curses_define_const(A_CHARTEXT);
#ifdef A_HORIZONTAL
    /* Document-const: A_HORIZONTAL
     *
     * Attribute mask:
     * horizontal highlight
     *
     * Check system curs_attr(3x) for support
     */
    rb_curses_define_const(A_HORIZONTAL);
#endif
#ifdef A_LEFT
    /* Document-const: A_LEFT
     *
     * Attribute mask:
     * left highlight
     *
     * Check system curs_attr(3x) for support
     */
    rb_curses_define_const(A_LEFT);
#endif
#ifdef A_LOW
    /* Document-const: A_LOW
     *
     * Attribute mask:
     * low highlight
     *
     * Check system curs_attr(3x) for support
     */
    rb_curses_define_const(A_LOW);
#endif
#ifdef A_RIGHT
    /* Document-const: A_RIGHT
     *
     * Attribute mask:
     * right highlight
     *
     * Check system curs_attr(3x) for support
     */
    rb_curses_define_const(A_RIGHT);
#endif
#ifdef A_TOP
    /* Document-const: A_TOP
     *
     * Attribute mask:
     * top highlight
     *
     * Check system curs_attr(3x) for support
     */
    rb_curses_define_const(A_TOP);
#endif
#ifdef A_VERTICAL
    /* Document-const: A_VERTICAL
     *
     * Attribute mask:
     * vertical highlight
     *
     * Check system curs_attr(3x) for support
     */
    rb_curses_define_const(A_VERTICAL);
#endif
    /* Document-const: A_COLOR
     *
     * Character attribute mask:
     * Bit-mask to extract color-pair field information
     *
     * See Curses.inch or Curses::Window.inch
     */
    rb_curses_define_const(A_COLOR);

#ifdef COLORS
    /*
     * Document-const: Curses::COLORS
     *
     * Number of the colors available
     */
    rb_curses_define_const(COLORS);
#endif
    /*
     * Document-const: Curses::COLOR_BLACK
     *
     * Value of the color black
     */
    rb_curses_define_const(COLOR_BLACK);
    /*
     * Document-const: COLOR_RED
     *
     * Value of the color red
     */
    rb_curses_define_const(COLOR_RED);
    /*
     * Document-const: COLOR_GREEN
     *
     * Value of the color green
     */
    rb_curses_define_const(COLOR_GREEN);
    /*
     * Document-const: COLOR_YELLOW
     *
     * Value of the color yellow
     */
    rb_curses_define_const(COLOR_YELLOW);
    /*
     * Document-const: COLOR_BLUE
     *
     * Value of the color blue
     */
    rb_curses_define_const(COLOR_BLUE);
    /*
     * Document-const: COLOR_MAGENTA
     *
     * Value of the color magenta
     */
    rb_curses_define_const(COLOR_MAGENTA);
    /*
     * Document-const: COLOR_CYAN
     *
     * Value of the color cyan
     */
    rb_curses_define_const(COLOR_CYAN);
    /*
     * Document-const: COLOR_WHITE
     *
     * Value of the color white
     */
    rb_curses_define_const(COLOR_WHITE);
#endif /* USE_COLOR */
#ifdef USE_MOUSE
#ifdef BUTTON1_PRESSED
    /* Document-const: BUTTON1_PRESSED
     *
     * Mouse event mask:
     * mouse button 1 down
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON1_PRESSED);
#endif
#ifdef BUTTON1_RELEASED
    /* Document-const: BUTTON1_RELEASED
     *
     * Mouse event mask:
     * mouse button 1 up
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON1_RELEASED);
#endif
#ifdef BUTTON1_CLICKED
    /* Document-const: BUTTON1_CLICKED
     *
     * Mouse event mask:
     * mouse button 1 clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON1_CLICKED);
#endif
#ifdef BUTTON1_DOUBLE_CLICKED
    /* Document-const: BUTTON1_DOUBLE_CLICKED
     *
     * Mouse event mask:
     * mouse button 1 double clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON1_DOUBLE_CLICKED);
#endif
#ifdef BUTTON1_TRIPLE_CLICKED
    /* Document-const: BUTTON1_TRIPLE_CLICKED
     *
     * Mouse event mask:
     * mouse button 1 triple clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON1_TRIPLE_CLICKED);
#endif
#ifdef BUTTON2_PRESSED
    /* Document-const: BUTTON2_PRESSED
     *
     * Mouse event mask:
     * mouse button 2 down
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON2_PRESSED);
#endif
#ifdef BUTTON2_RELEASED
    /* Document-const: BUTTON2_RELEASED
     *
     * Mouse event mask:
     * mouse button 2 up
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON2_RELEASED);
#endif
#ifdef BUTTON2_CLICKED
    /* Document-const: BUTTON2_CLICKED
     *
     * Mouse event mask:
     * mouse button 2 clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON2_CLICKED);
#endif
#ifdef BUTTON2_DOUBLE_CLICKED
    /* Document-const: BUTTON2_DOUBLE_CLICKED
     *
     * Mouse event mask:
     * mouse button 2 double clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON2_DOUBLE_CLICKED);
#endif
#ifdef BUTTON2_TRIPLE_CLICKED
    /* Document-const: BUTTON2_TRIPLE_CLICKED
     *
     * Mouse event mask:
     * mouse button 2 triple clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON2_TRIPLE_CLICKED);
#endif
#ifdef BUTTON3_PRESSED
    /* Document-const: BUTTON3_PRESSED
     *
     * Mouse event mask:
     * mouse button 3 down
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON3_PRESSED);
#endif
#ifdef BUTTON3_RELEASED
    /* Document-const: BUTTON3_RELEASED
     *
     * Mouse event mask:
     * mouse button 3 up
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON3_RELEASED);
#endif
#ifdef BUTTON3_CLICKED
    /* Document-const: BUTTON3_CLICKED
     *
     * Mouse event mask:
     * mouse button 3 clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON3_CLICKED);
#endif
#ifdef BUTTON3_DOUBLE_CLICKED
    /* Document-const: BUTTON3_DOUBLE_CLICKED
     *
     * Mouse event mask:
     * mouse button 3 double clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON3_DOUBLE_CLICKED);
#endif
#ifdef BUTTON3_TRIPLE_CLICKED
    /* Document-const: BUTTON3_TRIPLE_CLICKED
     *
     * Mouse event mask:
     * mouse button 3 triple clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON3_TRIPLE_CLICKED);
#endif
#ifdef BUTTON4_PRESSED
    /* Document-const: BUTTON4_PRESSED
     *
     * Mouse event mask:
     * mouse button 4 down
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON4_PRESSED);
#endif
#ifdef BUTTON4_RELEASED
    /* Document-const: BUTTON4_RELEASED
     *
     * Mouse event mask:
     * mouse button 4 up
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON4_RELEASED);
#endif
#ifdef BUTTON4_CLICKED
    /* Document-const: BUTTON4_CLICKED
     *
     * Mouse event mask:
     * mouse button 4 clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON4_CLICKED);
#endif
#ifdef BUTTON4_DOUBLE_CLICKED
    /* Document-const: BUTTON4_DOUBLE_CLICKED
     *
     * Mouse event mask:
     * mouse button 4 double clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON4_DOUBLE_CLICKED);
#endif
#ifdef BUTTON4_TRIPLE_CLICKED
    /* Document-const: BUTTON4_TRIPLE_CLICKED
     *
     * Mouse event mask:
     * mouse button 4 triple clicked
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON4_TRIPLE_CLICKED);
#endif
#ifdef BUTTON_SHIFT
    /* Document-const: BUTTON_SHIFT
     *
     * Mouse event mask:
     * shift was down during button state change
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON_SHIFT);
#endif
#ifdef BUTTON_CTRL
    /* Document-const: BUTTON_CTRL
     *
     * Mouse event mask:
     * control was down during button state change
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON_CTRL);
#endif
#ifdef BUTTON_ALT
    /* Document-const: BUTTON_ALT
     *
     * Mouse event mask:
     * alt was down during button state change
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(BUTTON_ALT);
#endif
#ifdef ALL_MOUSE_EVENTS
    /* Document-const: ALL_MOUSE_EVENTS
     *
     * Mouse event mask:
     * report all button state changes
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(ALL_MOUSE_EVENTS);
#endif
#ifdef REPORT_MOUSE_POSITION
    /* Document-const: REPORT_MOUSE_POSITION
     *
     * Mouse event mask:
     * report mouse movement
     *
     * See Curses.getmouse
     */
    rb_curses_define_const(REPORT_MOUSE_POSITION);
#endif
#endif /* USE_MOUSE */

#if defined(KEY_MOUSE) && defined(USE_MOUSE)
    /* Document-const: KEY_MOUSE
     * Mouse event read
     */
    /* Document-const: MOUSE
     * Mouse event read
     */
    rb_curses_define_const(KEY_MOUSE);
    rb_define_const(mKey, "MOUSE", INT2NUM(KEY_MOUSE));
#endif
#ifdef KEY_MIN
    /* Document-const: KEY_MIN
     * The minimum allowed curses key value.
     */
    /* Document-const: MIN
     * The minimum allowed curses key value.
     */
    rb_curses_define_const(KEY_MIN);
    rb_define_const(mKey, "MIN", INT2NUM(KEY_MIN));
#endif
#ifdef KEY_BREAK
    /* Document-const: KEY_BREAK
     * Break key
     */
    /* Document-const: BREAK
     * Break key
     */
    rb_curses_define_const(KEY_BREAK);
    rb_define_const(mKey, "BREAK", INT2NUM(KEY_BREAK));
#endif
#ifdef KEY_DOWN
    /* Document-const: KEY_DOWN
     * the down arrow key
     */
    /* Document-const: DOWN
     * the down arrow key
     */
    rb_curses_define_const(KEY_DOWN);
    rb_define_const(mKey, "DOWN", INT2NUM(KEY_DOWN));
#endif
#ifdef KEY_UP
    /* Document-const: KEY_UP
     * the up arrow key
     */
    /* Document-const: UP
     * the up arrow key
     */
    rb_curses_define_const(KEY_UP);
    rb_define_const(mKey, "UP", INT2NUM(KEY_UP));
#endif
#ifdef KEY_LEFT
    /* Document-const: KEY_LEFT
     * the left arrow key
     */
    /* Document-const: LEFT
     * the left arrow key
     */
    rb_curses_define_const(KEY_LEFT);
    rb_define_const(mKey, "LEFT", INT2NUM(KEY_LEFT));
#endif
#ifdef KEY_RIGHT
    /* Document-const: KEY_RIGHT
     * the right arrow key
     */
    /* Document-const: RIGHT
     * the right arrow key
     */
    rb_curses_define_const(KEY_RIGHT);
    rb_define_const(mKey, "RIGHT", INT2NUM(KEY_RIGHT));
#endif
#ifdef KEY_HOME
    /* Document-const: KEY_HOME
     * Home key (upward+left arrow)
     */
    /* Document-const: HOME
     * Home key (upward+left arrow)
     */
    rb_curses_define_const(KEY_HOME);
    rb_define_const(mKey, "HOME", INT2NUM(KEY_HOME));
#endif
#ifdef KEY_BACKSPACE
    /* Document-const: KEY_BACKSPACE
     * Backspace
     */
    /* Document-const: BACKSPACE
     * Backspace
     */
    rb_curses_define_const(KEY_BACKSPACE);
    rb_define_const(mKey, "BACKSPACE", INT2NUM(KEY_BACKSPACE));
#endif
#ifdef KEY_F
    /* KEY_F(n) : 0 <= n <= 63 */
    {
	int i;
	char c[8];
	for (i=0; i<64; i++) {
	    sprintf(c, "KEY_F%d", i);
	    rb_define_const(mCurses, c, INT2NUM(KEY_F(i)));
	    sprintf(c, "F%d", i);
	    rb_define_const(mKey, c, INT2NUM(KEY_F(i)));
	}
    }
#endif
#ifdef KEY_DL
    /* Document-const: KEY_DL
     * Delete line
     */
    /* Document-const: DL
     * Delete line
     */
    rb_curses_define_const(KEY_DL);
    rb_define_const(mKey, "DL", INT2NUM(KEY_DL));
#endif
#ifdef KEY_IL
    /* Document-const: KEY_IL
     * Insert line
     */
    /* Document-const: IL
     * Insert line
     */
    rb_curses_define_const(KEY_IL);
    rb_define_const(mKey, "IL", INT2NUM(KEY_IL));
#endif
#ifdef KEY_DC
    /* Document-const: KEY_DC
     * Delete character
     */
    /* Document-const: DC
     * Delete character
     */
    rb_curses_define_const(KEY_DC);
    rb_define_const(mKey, "DC", INT2NUM(KEY_DC));
#endif
#ifdef KEY_IC
    /* Document-const: KEY_IC
     * Insert char or enter insert mode
     */
    /* Document-const: IC
     * Insert char or enter insert mode
     */
    rb_curses_define_const(KEY_IC);
    rb_define_const(mKey, "IC", INT2NUM(KEY_IC));
#endif
#ifdef KEY_EIC
    /* Document-const: KEY_EIC
     * Enter insert char mode
     */
    /* Document-const: EIC
     * Enter insert char mode
     */
    rb_curses_define_const(KEY_EIC);
    rb_define_const(mKey, "EIC", INT2NUM(KEY_EIC));
#endif
#ifdef KEY_CLEAR
    /* Document-const: KEY_CLEAR
     * Clear Screen
     */
    /* Document-const: CLEAR
     * Clear Screen
     */
    rb_curses_define_const(KEY_CLEAR);
    rb_define_const(mKey, "CLEAR", INT2NUM(KEY_CLEAR));
#endif
#ifdef KEY_EOS
    /* Document-const: KEY_EOS
     * Clear to end of screen
     */
    /* Document-const: EOS
     * Clear to end of screen
     */
    rb_curses_define_const(KEY_EOS);
    rb_define_const(mKey, "EOS", INT2NUM(KEY_EOS));
#endif
#ifdef KEY_EOL
    /* Document-const: KEY_EOL
     * Clear to end of line
     */
    /* Document-const: EOL
     * Clear to end of line
     */
    rb_curses_define_const(KEY_EOL);
    rb_define_const(mKey, "EOL", INT2NUM(KEY_EOL));
#endif
#ifdef KEY_SF
    /* Document-const: KEY_SF
     * Scroll 1 line forward
     */
    /* Document-const: SF
     * Scroll 1 line forward
     */
    rb_curses_define_const(KEY_SF);
    rb_define_const(mKey, "SF", INT2NUM(KEY_SF));
#endif
#ifdef KEY_SR
    /* Document-const: KEY_SR
     * Scroll 1 line backware (reverse)
     */
    /* Document-const: SR
     * Scroll 1 line backware (reverse)
     */
    rb_curses_define_const(KEY_SR);
    rb_define_const(mKey, "SR", INT2NUM(KEY_SR));
#endif
#ifdef KEY_NPAGE
    /* Document-const: KEY_NPAGE
     * Next page
     */
    /* Document-const: NPAGE
     * Next page
     */
    rb_curses_define_const(KEY_NPAGE);
    rb_define_const(mKey, "NPAGE", INT2NUM(KEY_NPAGE));
#endif
#ifdef KEY_PPAGE
    /* Document-const: KEY_PPAGE
     * Previous page
     */
    /* Document-const: PPAGE
     * Previous page
     */
    rb_curses_define_const(KEY_PPAGE);
    rb_define_const(mKey, "PPAGE", INT2NUM(KEY_PPAGE));
#endif
#ifdef KEY_STAB
    /* Document-const: KEY_STAB
     * Set tab
     */
    /* Document-const: STAB
     * Set tab
     */
    rb_curses_define_const(KEY_STAB);
    rb_define_const(mKey, "STAB", INT2NUM(KEY_STAB));
#endif
#ifdef KEY_CTAB
    /* Document-const: KEY_CTAB
     * Clear tab
     */
    /* Document-const: CTAB
     * Clear tab
     */
    rb_curses_define_const(KEY_CTAB);
    rb_define_const(mKey, "CTAB", INT2NUM(KEY_CTAB));
#endif
#ifdef KEY_CATAB
    /* Document-const: KEY_CATAB
     * Clear all tabs
     */
    /* Document-const: CATAB
     * Clear all tabs
     */
    rb_curses_define_const(KEY_CATAB);
    rb_define_const(mKey, "CATAB", INT2NUM(KEY_CATAB));
#endif
#ifdef KEY_ENTER
    /* Document-const: KEY_ENTER
     * Enter or send
     */
    /* Document-const: ENTER
     * Enter or send
     */
    rb_curses_define_const(KEY_ENTER);
    rb_define_const(mKey, "ENTER", INT2NUM(KEY_ENTER));
#endif
#ifdef KEY_SRESET
    /* Document-const: KEY_SRESET
     * Soft (partial) reset
     */
    /* Document-const: SRESET
     * Soft (partial) reset
     */
    rb_curses_define_const(KEY_SRESET);
    rb_define_const(mKey, "SRESET", INT2NUM(KEY_SRESET));
#endif
#ifdef KEY_RESET
    /* Document-const: KEY_RESET
     * Reset or hard reset
     */
    /* Document-const: RESET
     * Reset or hard reset
     */
    rb_curses_define_const(KEY_RESET);
    rb_define_const(mKey, "RESET", INT2NUM(KEY_RESET));
#endif
#ifdef KEY_PRINT
    /* Document-const: KEY_PRINT
     * Print or copy
     */
    /* Document-const: PRINT
     * Print or copy
     */
    rb_curses_define_const(KEY_PRINT);
    rb_define_const(mKey, "PRINT", INT2NUM(KEY_PRINT));
#endif
#ifdef KEY_LL
    /* Document-const: KEY_LL
     * Home down or bottom (lower left)
     */
    /* Document-const: LL
     * Home down or bottom (lower left)
     */
    rb_curses_define_const(KEY_LL);
    rb_define_const(mKey, "LL", INT2NUM(KEY_LL));
#endif
#ifdef KEY_A1
    /* Document-const: KEY_A1
     * Upper left of keypad
     */
    /* Document-const: A1
     * Upper left of keypad
     */
    rb_curses_define_const(KEY_A1);
    rb_define_const(mKey, "A1", INT2NUM(KEY_A1));
#endif
#ifdef KEY_A3
    /* Document-const: KEY_A3
     * Upper right of keypad
     */
    /* Document-const: A3
     * Upper right of keypad
     */
    rb_curses_define_const(KEY_A3);
    rb_define_const(mKey, "A3", INT2NUM(KEY_A3));
#endif
#ifdef KEY_B2
    /* Document-const: KEY_B2
     * Center of keypad
     */
    /* Document-const: B2
     * Center of keypad
     */
    rb_curses_define_const(KEY_B2);
    rb_define_const(mKey, "B2", INT2NUM(KEY_B2));
#endif
#ifdef KEY_C1
    /* Document-const: KEY_C1
     * Lower left of keypad
     */
    /* Document-const: C1
     * Lower left of keypad
     */
    rb_curses_define_const(KEY_C1);
    rb_define_const(mKey, "C1", INT2NUM(KEY_C1));
#endif
#ifdef KEY_C3
    /* Document-const: KEY_C3
     * Lower right of keypad
     */
    /* Document-const: C3
     * Lower right of keypad
     */
    rb_curses_define_const(KEY_C3);
    rb_define_const(mKey, "C3", INT2NUM(KEY_C3));
#endif
#ifdef KEY_BTAB
    /* Document-const: BTAB
     * Back tab key
     */
    /* Document-const: KEY_BTAB
     * Back tab key
     */
    rb_curses_define_const(KEY_BTAB);
    rb_define_const(mKey, "BTAB", INT2NUM(KEY_BTAB));
#endif
#ifdef KEY_BEG
    /* Document-const: KEY_BEG
     * Beginning key
     */
    /* Document-const: BEG
     * Beginning key
     */
    rb_curses_define_const(KEY_BEG);
    rb_define_const(mKey, "BEG", INT2NUM(KEY_BEG));
#endif
#ifdef KEY_CANCEL
    /* Document-const: KEY_CANCEL
     * Cancel key
     */
    /* Document-const: CANCEL
     * Cancel key
     */
    rb_curses_define_const(KEY_CANCEL);
    rb_define_const(mKey, "CANCEL", INT2NUM(KEY_CANCEL));
#endif
#ifdef KEY_CLOSE
    /* Document-const: KEY_CLOSE
     * Close key
     */
    /* Document-const: CLOSE
     * Close key
     */
    rb_curses_define_const(KEY_CLOSE);
    rb_define_const(mKey, "CLOSE", INT2NUM(KEY_CLOSE));
#endif
#ifdef KEY_COMMAND
    /* Document-const: KEY_COMMAND
     * Cmd (command) key
     */
    /* Document-const: COMMAND
     * Cmd (command) key
     */
    rb_curses_define_const(KEY_COMMAND);
    rb_define_const(mKey, "COMMAND", INT2NUM(KEY_COMMAND));
#endif
#ifdef KEY_COPY
    /* Document-const: KEY_COPY
     * Copy key
     */
    /* Document-const: COPY
     * Copy key
     */
    rb_curses_define_const(KEY_COPY);
    rb_define_const(mKey, "COPY", INT2NUM(KEY_COPY));
#endif
#ifdef KEY_CREATE
    /* Document-const: KEY_CREATE
     * Create key
     */
    /* Document-const: CREATE
     * Create key
     */
    rb_curses_define_const(KEY_CREATE);
    rb_define_const(mKey, "CREATE", INT2NUM(KEY_CREATE));
#endif
#ifdef KEY_END
    /* Document-const: KEY_END
     * End key
     */
    /* Document-const: END
     * End key
     */
    rb_curses_define_const(KEY_END);
    rb_define_const(mKey, "END", INT2NUM(KEY_END));
#endif
#ifdef KEY_EXIT
    /* Document-const: KEY_EXIT
     * Exit key
     */
    /* Document-const: EXIT
     * Exit key
     */
    rb_curses_define_const(KEY_EXIT);
    rb_define_const(mKey, "EXIT", INT2NUM(KEY_EXIT));
#endif
#ifdef KEY_FIND
    /* Document-const: KEY_FIND
     * Find key
     */
    /* Document-const: FIND
     * Find key
     */
    rb_curses_define_const(KEY_FIND);
    rb_define_const(mKey, "FIND", INT2NUM(KEY_FIND));
#endif
#ifdef KEY_HELP
    /* Document-const: KEY_HELP
     * Help key
     */
    /* Document-const: HELP
     * Help key
     */
    rb_curses_define_const(KEY_HELP);
    rb_define_const(mKey, "HELP", INT2NUM(KEY_HELP));
#endif
#ifdef KEY_MARK
    /* Document-const: KEY_MARK
     * Mark key
     */
    /* Document-const: MARK
     * Mark key
     */
    rb_curses_define_const(KEY_MARK);
    rb_define_const(mKey, "MARK", INT2NUM(KEY_MARK));
#endif
#ifdef KEY_MESSAGE
    /* Document-const: KEY_MESSAGE
     * Message key
     */
    /* Document-const: MESSAGE
     * Message key
     */
    rb_curses_define_const(KEY_MESSAGE);
    rb_define_const(mKey, "MESSAGE", INT2NUM(KEY_MESSAGE));
#endif
#ifdef KEY_MOVE
    /* Document-const: KEY_MOVE
     * Move key
     */
    /* Document-const: MOVE
     * Move key
     */
    rb_curses_define_const(KEY_MOVE);
    rb_define_const(mKey, "MOVE", INT2NUM(KEY_MOVE));
#endif
#ifdef KEY_NEXT
    /* Document-const: KEY_NEXT
     * Next object key
     */
    /* Document-const: NEXT
     * Next object key
     */
    rb_curses_define_const(KEY_NEXT);
    rb_define_const(mKey, "NEXT", INT2NUM(KEY_NEXT));
#endif
#ifdef KEY_OPEN
    /* Document-const: KEY_OPEN
     * Open key
     */
    /* Document-const: OPEN
     * Open key
     */
    rb_curses_define_const(KEY_OPEN);
    rb_define_const(mKey, "OPEN", INT2NUM(KEY_OPEN));
#endif
#ifdef KEY_OPTIONS
    /* Document-const: KEY_OPTIONS
     * Options key
     */
    /* Document-const: OPTIONS
     * Options key
     */
    rb_curses_define_const(KEY_OPTIONS);
    rb_define_const(mKey, "OPTIONS", INT2NUM(KEY_OPTIONS));
#endif
#ifdef KEY_PREVIOUS
    /* Document-const: KEY_PREVIOUS
     * Previous object key
     */
    /* Document-const: PREVIOUS
     * Previous object key
     */
    rb_curses_define_const(KEY_PREVIOUS);
    rb_define_const(mKey, "PREVIOUS", INT2NUM(KEY_PREVIOUS));
#endif
#ifdef KEY_REDO
    /* Document-const: KEY_REDO
     * Redo key
     */
    /* Document-const: REDO
     * Redo key
     */
    rb_curses_define_const(KEY_REDO);
    rb_define_const(mKey, "REDO", INT2NUM(KEY_REDO));
#endif
#ifdef KEY_REFERENCE
    /* Document-const: KEY_REFERENCE
     * Reference key
     */
    /* Document-const: REFERENCE
     * Reference key
     */
    rb_curses_define_const(KEY_REFERENCE);
    rb_define_const(mKey, "REFERENCE", INT2NUM(KEY_REFERENCE));
#endif
#ifdef KEY_REFRESH
    /* Document-const: KEY_REFRESH
     * Refresh key
     */
    /* Document-const: REFRESH
     * Refresh key
     */
    rb_curses_define_const(KEY_REFRESH);
    rb_define_const(mKey, "REFRESH", INT2NUM(KEY_REFRESH));
#endif
#ifdef KEY_REPLACE
    /* Document-const: KEY_REPLACE
     * Replace key
     */
    /* Document-const: REPLACE
     * Replace key
     */
    rb_curses_define_const(KEY_REPLACE);
    rb_define_const(mKey, "REPLACE", INT2NUM(KEY_REPLACE));
#endif
#ifdef KEY_RESTART
    /* Document-const: KEY_RESTART
     * Restart key
     */
    /* Document-const: RESTART
     * Restart key
     */
    rb_curses_define_const(KEY_RESTART);
    rb_define_const(mKey, "RESTART", INT2NUM(KEY_RESTART));
#endif
#ifdef KEY_RESUME
    /* Document-const: KEY_RESUME
     * Resume key
     */
    /* Document-const: RESUME
     * Resume key
     */
    rb_curses_define_const(KEY_RESUME);
    rb_define_const(mKey, "RESUME", INT2NUM(KEY_RESUME));
#endif
#ifdef KEY_SAVE
    /* Document-const: KEY_SAVE
     * Save key
     */
    /* Document-const: SAVE
     * Save key
     */
    rb_curses_define_const(KEY_SAVE);
    rb_define_const(mKey, "SAVE", INT2NUM(KEY_SAVE));
#endif
#ifdef KEY_SBEG
    /* Document-const: KEY_SBEG
     * Shifted beginning key
     */
    /* Document-const: SBEG
     * Shifted beginning key
     */
    rb_curses_define_const(KEY_SBEG);
    rb_define_const(mKey, "SBEG", INT2NUM(KEY_SBEG));
#endif
#ifdef KEY_SCANCEL
    /* Document-const: KEY_SCANCEL
     * Shifted cancel key
     */
    /* Document-const: SCANCEL
     * Shifted cancel key
     */
    rb_curses_define_const(KEY_SCANCEL);
    rb_define_const(mKey, "SCANCEL", INT2NUM(KEY_SCANCEL));
#endif
#ifdef KEY_SCOMMAND
    /* Document-const: KEY_SCOMMAND
     * Shifted command key
     */
    /* Document-const: SCOMMAND
     * Shifted command key
     */
    rb_curses_define_const(KEY_SCOMMAND);
    rb_define_const(mKey, "SCOMMAND", INT2NUM(KEY_SCOMMAND));
#endif
#ifdef KEY_SCOPY
    /* Document-const: KEY_SCOPY
     * Shifted copy key
     */
    /* Document-const: SCOPY
     * Shifted copy key
     */
    rb_curses_define_const(KEY_SCOPY);
    rb_define_const(mKey, "SCOPY", INT2NUM(KEY_SCOPY));
#endif
#ifdef KEY_SCREATE
    /* Document-const: KEY_SCREATE
     * Shifted create key
     */
    /* Document-const: SCREATE
     * Shifted create key
     */
    rb_curses_define_const(KEY_SCREATE);
    rb_define_const(mKey, "SCREATE", INT2NUM(KEY_SCREATE));
#endif
#ifdef KEY_SDC
    /* Document-const: KEY_SDC
     * Shifted delete char key
     */
    /* Document-const: SDC
     * Shifted delete char key
     */
    rb_curses_define_const(KEY_SDC);
    rb_define_const(mKey, "SDC", INT2NUM(KEY_SDC));
#endif
#ifdef KEY_SDL
    /* Document-const: KEY_SDL
     * Shifted delete line key
     */
    /* Document-const: SDL
     * Shifted delete line key
     */
    rb_curses_define_const(KEY_SDL);
    rb_define_const(mKey, "SDL", INT2NUM(KEY_SDL));
#endif
#ifdef KEY_SELECT
    /* Document-const: KEY_SELECT
     * Select key
     */
    /* Document-const: SELECT
     * Select key
     */
    rb_curses_define_const(KEY_SELECT);
    rb_define_const(mKey, "SELECT", INT2NUM(KEY_SELECT));
#endif
#ifdef KEY_SEND
    /* Document-const: KEY_SEND
     * Shifted end key
     */
    /* Document-const: SEND
     * Shifted end key
     */
    rb_curses_define_const(KEY_SEND);
    rb_define_const(mKey, "SEND", INT2NUM(KEY_SEND));
#endif
#ifdef KEY_SEOL
    /* Document-const: KEY_SEOL
     * Shifted clear line key
     */
    /* Document-const: SEOL
     * Shifted clear line key
     */
    rb_curses_define_const(KEY_SEOL);
    rb_define_const(mKey, "SEOL", INT2NUM(KEY_SEOL));
#endif
#ifdef KEY_SEXIT
    /* Document-const: KEY_SEXIT
     * Shifted exit key
     */
    /* Document-const: SEXIT
     * Shifted exit key
     */
    rb_curses_define_const(KEY_SEXIT);
    rb_define_const(mKey, "SEXIT", INT2NUM(KEY_SEXIT));
#endif
#ifdef KEY_SFIND
    /* Document-const: KEY_SFIND
     * Shifted find key
     */
    /* Document-const: SFIND
     * Shifted find key
     */
    rb_curses_define_const(KEY_SFIND);
    rb_define_const(mKey, "SFIND", INT2NUM(KEY_SFIND));
#endif
#ifdef KEY_SHELP
    /* Document-const: KEY_SHELP
     * Shifted help key
     */
    /* Document-const: SHELP
     * Shifted help key
     */
    rb_curses_define_const(KEY_SHELP);
    rb_define_const(mKey, "SHELP", INT2NUM(KEY_SHELP));
#endif
#ifdef KEY_SHOME
    /* Document-const: KEY_SHOME
     * Shifted home key
     */
    /* Document-const: SHOME
     * Shifted home key
     */
    rb_curses_define_const(KEY_SHOME);
    rb_define_const(mKey, "SHOME", INT2NUM(KEY_SHOME));
#endif
#ifdef KEY_SIC
    /* Document-const: KEY_SIC
     * Shifted input key
     */
    /* Document-const: SIC
     * Shifted input key
     */
    rb_curses_define_const(KEY_SIC);
    rb_define_const(mKey, "SIC", INT2NUM(KEY_SIC));
#endif
#ifdef KEY_SLEFT
    /* Document-const: KEY_SLEFT
     * Shifted left arrow key
     */
    /* Document-const: SLEFT
     * Shifted left arrow key
     */
    rb_curses_define_const(KEY_SLEFT);
    rb_define_const(mKey, "SLEFT", INT2NUM(KEY_SLEFT));
#endif
#ifdef KEY_SMESSAGE
    /* Document-const: KEY_SMESSAGE
     * Shifted message key
     */
    /* Document-const: SMESSAGE
     * Shifted message key
     */
    rb_curses_define_const(KEY_SMESSAGE);
    rb_define_const(mKey, "SMESSAGE", INT2NUM(KEY_SMESSAGE));
#endif
#ifdef KEY_SMOVE
    /* Document-const: KEY_SMOVE
     * Shifted move key
     */
    /* Document-const: SMOVE
     * Shifted move key
     */
    rb_curses_define_const(KEY_SMOVE);
    rb_define_const(mKey, "SMOVE", INT2NUM(KEY_SMOVE));
#endif
#ifdef KEY_SNEXT
    /* Document-const: KEY_SNEXT
     * Shifted next key
     */
    /* Document-const: SNEXT
     * Shifted next key
     */
    rb_curses_define_const(KEY_SNEXT);
    rb_define_const(mKey, "SNEXT", INT2NUM(KEY_SNEXT));
#endif
#ifdef KEY_SOPTIONS
    /* Document-const: KEY_SOPTIONS
     * Shifted options key
     */
    /* Document-const: SOPTIONS
     * Shifted options key
     */
    rb_curses_define_const(KEY_SOPTIONS);
    rb_define_const(mKey, "SOPTIONS", INT2NUM(KEY_SOPTIONS));
#endif
#ifdef KEY_SPREVIOUS
    /* Document-const: KEY_SPREVIOUS
     * Shifted previous key
     */
    /* Document-const: SPREVIOUS
     * Shifted previous key
     */
    rb_curses_define_const(KEY_SPREVIOUS);
    rb_define_const(mKey, "SPREVIOUS", INT2NUM(KEY_SPREVIOUS));
#endif
#ifdef KEY_SPRINT
    /* Document-const: KEY_SPRINT
     * Shifted print key
     */
    /* Document-const: SPRINT
     * Shifted print key
     */
    rb_curses_define_const(KEY_SPRINT);
    rb_define_const(mKey, "SPRINT", INT2NUM(KEY_SPRINT));
#endif
#ifdef KEY_SREDO
    /* Document-const: KEY_SREDO
     * Shifted redo key
     */
    /* Document-const: SREDO
     * Shifted redo key
     */
    rb_curses_define_const(KEY_SREDO);
    rb_define_const(mKey, "SREDO", INT2NUM(KEY_SREDO));
#endif
#ifdef KEY_SREPLACE
    /* Document-const: KEY_SREPLACE
     * Shifted replace key
     */
    /* Document-const: SREPLACE
     * Shifted replace key
     */
    rb_curses_define_const(KEY_SREPLACE);
    rb_define_const(mKey, "SREPLACE", INT2NUM(KEY_SREPLACE));
#endif
#ifdef KEY_SRIGHT
    /* Document-const: KEY_SRIGHT
     * Shifted right arrow key
     */
    /* Document-const: SRIGHT
     * Shifted right arrow key
     */
    rb_curses_define_const(KEY_SRIGHT);
    rb_define_const(mKey, "SRIGHT", INT2NUM(KEY_SRIGHT));
#endif
#ifdef KEY_SRSUME
    /* Document-const: KEY_SRSUME
     * Shifted resume key
     */
    /* Document-const: SRSUME
     * Shifted resume key
     */
    rb_curses_define_const(KEY_SRSUME);
    rb_define_const(mKey, "SRSUME", INT2NUM(KEY_SRSUME));
#endif
#ifdef KEY_SSAVE
    /* Document-const: KEY_SSAVE
     * Shifted save key
     */
    /* Document-const: SSAVE
     * Shifted save key
     */
    rb_curses_define_const(KEY_SSAVE);
    rb_define_const(mKey, "SSAVE", INT2NUM(KEY_SSAVE));
#endif
#ifdef KEY_SSUSPEND
    /* Document-const: KEY_SSUSPEND
     * Shifted suspend key
     */
    /* Document-const: SSUSPEND
     * Shifted suspend key
     */
    rb_curses_define_const(KEY_SSUSPEND);
    rb_define_const(mKey, "SSUSPEND", INT2NUM(KEY_SSUSPEND));
#endif
#ifdef KEY_SUNDO
    /* Document-const: KEY_SUNDO
     * Shifted undo key
     */
    /* Document-const: SUNDO
     * Shifted undo key
     */
    rb_curses_define_const(KEY_SUNDO);
    rb_define_const(mKey, "SUNDO", INT2NUM(KEY_SUNDO));
#endif
#ifdef KEY_SUSPEND
    /* Document-const: KEY_SUSPEND
     * Suspend key
     */
    /* Document-const: SUSPEND
     * Suspend key
     */
    rb_curses_define_const(KEY_SUSPEND);
    rb_define_const(mKey, "SUSPEND", INT2NUM(KEY_SUSPEND));
#endif
#ifdef KEY_UNDO
    /* Document-const: KEY_UNDO
     * Undo key
     */
    /* Document-const: UNDO
     * Undo key
     */
    rb_curses_define_const(KEY_UNDO);
    rb_define_const(mKey, "UNDO", INT2NUM(KEY_UNDO));
#endif
#ifdef KEY_RESIZE
    /* Document-const: KEY_RESIZE
     * Screen Resized
     */
    /* Document-const: RESIZE
     * Screen Resized
     */
    rb_curses_define_const(KEY_RESIZE);
    rb_define_const(mKey, "RESIZE", INT2NUM(KEY_RESIZE));
#endif
#ifdef KEY_MAX
    /* Document-const: KEY_MAX
     * The maximum allowed curses key value.
     */
    /* Document-const: MAX
     * The maximum allowed curses key value.
     */
    rb_curses_define_const(KEY_MAX);
    rb_define_const(mKey, "MAX", INT2NUM(KEY_MAX));
#endif
    {
	int c;
	char name[] = "KEY_CTRL_x";
	for (c = 'A'; c <= 'Z'; c++) {
	    name[sizeof(name) - 2] = c;
	    rb_define_const(mCurses, name, INT2FIX(c - 'A' + 1));
	}
    }
#undef rb_curses_define_const

    rb_set_end_proc(curses_finalize, 0);
}
