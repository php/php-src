<?
#	msession-test.php
#	This is a test page for msession functions.
#	most msession functions are used in this page with
#	the exception of msession_get_data, and msession_set_data
#	which are used implicitly with the PHP session
#	extension.
#
# 
#

# Start the session system, this will connect to msession
# as configured in PHP.INI.
#
# Start sessions, this will set a cookie.
session_start();

# Now, optional, use msession_uniq() to create a guarenteed
# uniq session name.
# 
if(!$HTTP_COOKIE_VARS["PHPSESSID"])
{
	# Use uniq to create the session. This is guarenteed to be
	# uniq in the server.
	$sid = msession_uniq(32);
	setcookie ("PHPSESSID", $sid);
	session_id($sid);
	$HTTP_COOKIE_VARS["PHPSESSID"] = $sid;
	# New session, set some variables
	if(0) // One at a time
	{
		echo "Set Variable: " . msession_set($sid, 'time',time()) ."<p>\n";
		echo "Set Variable: " . msession_set($sid, 'name1','test1') ."<p>\n";
		echo "Set Variable: " . msession_set($sid, 'name2','test2') ."<p>\n";
		echo "Set Variable: " . msession_set($sid, 'name3','test3') ."<p>\n";
		echo "Set Variable: " . msession_set($sid, 'name4','test4') ."<p>\n";
		echo "Set Variable: " . msession_set($sid, 'name5','test5') ."<p>\n";
		echo "Set Variable: " . msession_set($sid, 'name6','test6') ."<p>\n";
		echo "Set Variable: " . msession_set($sid, 'name7','test7') ."<p>\n";
	}
	else // All at once in an array
	{
		$setarray = array();
		$setarray['time']=time();
		$setarray['name1'] = 'test1';
		$setarray['name2'] = 'test2';
		$setarray['name3'] = 'test3';
		$setarray['name4'] = 'test4';
		$setarray['name5'] = 'test5';
		$setarray['name6'] = 'test6';
		$setarray['name7'] = 'test7';
		msession_set_array($sid, $setarray);
	}
}
else
{
	$sid = $HTTP_COOKIE_VARS["PHPSESSID"];
}

#This makes a link between the variable $count and the
# session variable "count"
session_register("count");

$count ++;

# Output some information.
echo "sid: " . $sid . "<br>\n";
echo "Session Count: " . $count . "<br>\n";

# Use msession_randstr() to produce a random string.
# A valid string of n characters of jibberish is returned.
echo "Random String: " . msession_randstr(32) . "<br>\n";

# This is a thread safe increment, unlike PHP's session, many web servers
# can be updating this variable and collisions are managed.
# (for this to work, older versions of msessiond must be started with "-g globals"
#  newer versions create it by default)
echo "Global Count: " . msession_inc(globals, "counter") . "<br>\n";

# This gets a count of active sessions.
echo "Total active sessions: " . msession_count() . "<br>\n";

# This gets all the variables for a user in an associative array.
$varray = msession_get_array($sid);

if(!$varray)
	echo "Get variable array: Failed<br>\n";

# Display all the user's variables
$arraykeys = array_keys($varray);
for($i=0; $arraykeys[$i]; $i++)
	echo "Key: " . $arraykeys[ $i ] ." = " .$varray[$arraykeys[$i]] ."<br>\n";

	
# Find a list of all sessions with same name/value pair
$array = msession_find('name1', 'test1');

#display the sessions
for($i=0; $array[$i]; $i++)
	echo "Similar Sessions: " . $i . " " . $array[$i] . "<br>\n";

# Find all the sessions which have the variable "time" set.
$vararray = msession_listvar('time');

$arraykeys = array_keys($vararray);

for($i=0; $arraykeys[$i]; $i++)
	echo "Key: " . $arraykeys[ $i ] ." = " .$vararray[$arraykeys[$i]] ."<br>\n";

# msession can support a personality plugin, this is an escape call directly
# into the plugin's REQ_ESCAPE function.
echo "Call the plugin: " . msession_plugin($sid, 3, "test"). "<br>\n";

# msession also supprts function-only plugins. this is a call into the demo
# plugin (funct.so) which returns the uptime of the msessiond process. 
echo "Call the function: " . msession_call('fntest', "1","2", "3", "4") ."<br>\n"; 

#List ALL sessions on the system
$sarray = msession_list();

for($i=0; $sarray[$i]; $i++)
	echo "Sessions: " . $i . " " . $sarray[$i] . "<br>\n";

?>

