<?php
/*
 * This is a simple email viewer.
 * make sure that $filename points to a file containing an email message and
 * load this page in your browser.
 * You will be able to choose a part to view.
 * */

#$filename = "/home/CLIENTWEB/worlddo/mimetests/namib.rfc822";
$filename = "/home/CLIENTWEB/worlddo/mimetests/uumsg";
#$filename = "/home/CLIENTWEB/worlddo/mimetests/segblob.txt";
#$filename = "yourmessage.txt";

/* parse the message and return a mime message resource */
$mime = mailparse_msg_parse_file($filename);
/* return an array of message parts - this contsists of the names of the parts
 * only */
$struct = mailparse_msg_get_structure($mime);

echo "<table>\n";
/* print a choice of sections */
foreach($struct as $st)	{
	echo "<tr>\n";
	echo "<td><a href=\"$PHP_SELF?showpart=$st\">$st</a></td>\n";
	/* get a handle on the message resource for a subsection */
	$section = mailparse_msg_get_part($mime, $st);
	/* get content-type, encoding and header information for that section */
	$info = mailparse_msg_get_part_data($section);
	echo "\n";
	echo "<td>" . $info["content-type"] . "</td>\n";
	echo "<td>" . $info["content-disposition"] . "</td>\n";
	echo "<td>" . $info["disposition-filename"] . "</td>\n";
	echo "<td>" . $info["charset"] . "</td>\n";
	echo "</tr>";
}
echo "</table>";

/* if we were called to display a part, do so now */
if ($showpart)	{
	/* get a handle on the message resource for the desired part */
	$sec = mailparse_msg_get_part($mime, $showpart);

	echo "<table border=1><tr><th>Section $showpart</th></tr><tr><td>";
	ob_start();
	/* extract the part from the message file and dump it to the output buffer
	 * */
	mailparse_msg_extract_part_file($sec, $filename);
	$contents = ob_get_contents();
	ob_end_clean();
	/* quote the message for safe display in a browser */
	echo nl2br(htmlentities($contents)) . "</td></tr></table>";;
}
?>
