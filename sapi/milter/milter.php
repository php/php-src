<?php
/**
 * example milter script
 *
 * run: php-milter -D -p /path/to/sock milter.php
 *
 * for details on how to set up sendmail and configure the milter see
 * http://www.sendmail.com/partner/resources/development/milter_api/
 *
 * for api details see
 * http://www.sendmail.com/partner/resources/development/milter_api/api.html
 *
 * below is a list of all callbacks, that are available through the milter sapi,
 * if you leave one or more out they simply won't get called (e.g. if you secify an
 * empty php file, the milter would do nothing :)
 */

/**
 * this function is called once on sapi startup,
 * here you can specify the actions the filter may take
 *
 * see http://www.sendmail.com/partner/resources/development/milter_api/smfi_register.html#flags
 */
function milter_init() {
  smfi_setflags(SMFIF_ADDHDRS);
}

/**
 * is called once, at the start of each SMTP connection
 */
function milter_connect($connect)
{
}

/**
 * is called whenever the client sends a HELO/EHLO command.
 * It may therefore be called between zero and three times.
 */
function milter_helo($helo)
{
}

/**
 * is called once at the beginning of each message,
 * before milter_envrcpt.
 */
function milter_envfrom($args)
{
}

/**
 * is called once per recipient, hence one or more times per message,
 * immediately after milter_envfrom
 */
function milter_envrcpt($args)
{
}

/**
 * is called zero or more times between milter_envrcpt and milter_eoh,
 * once per message header
 */
function milter_header($header, $value)
{
}

/**
 * is called once after all headers have been sent and processed.
 */
function milter_eoh()
{
}

/**
 * is called zero or more times between milter_eoh and milter_eom.
 */
function mlfi_body($bodypart)
{
}

/**
 * is called once after all calls to milter_body for a given message.
 * most of the api functions, that alter the message can only be called
 * within this callback.
 */
function milter_eom()
{
  /* add PHP header to the message */
  smfi_addheader("X-PHP", phpversion());	
}

/**
 * may be called at any time during message processing
 * (i.e. between some message-oriented routine and milter_eom).
 */
function mlfi_abort()
{
}

/**
 * is always called once at the end of each connection.
 */
function mlfi_close()
{
}
?>
