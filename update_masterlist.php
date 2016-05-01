<?php
define('MIN_SERVERS_FOR_REWRITE', 500);
define('MASTERLIST_SERVERS_PER_PAGE', 25);
define('MASTERLIST_QUERY_LINK', 'http://monitor.sacnr.com/ajax/get_server_list.php?start=');
define('MASTERLIST_FILE', 'masterlist.txt');
define('MASTERLIST_FILE_TEMP', 'masterlist.tmp');
define('LOG_EXECTIME_FILE', 'execinfo.log');
define('EXEC_TIMELIMIT_MINUTES', 2);

function get_string_between($string, $start, $end, $offset)
{
    $string = ' ' . $string;
    $ini = strpos($string, $start, $offset);
    if ($ini == 0) return '';
    $ini += strlen($start);
    $len = strpos($string, $end, $ini) - $ini;
    return substr($string, $ini, $len);
}

set_time_limit(EXEC_TIMELIMIT_MINUTES*60);
$start_time = microtime(true);
$master = fopen(MASTERLIST_FILE_TEMP, 'w');
$i = 0;
$servers = 0;

while(1)
{
	$page = file_get_contents(MASTERLIST_QUERY_LINK.$i);
	
	$pos = strpos($page, '<a href="samp://');
	if($pos === false) 
		break;
	
	while($pos !== false)
	{
		$ip = get_string_between($page, '<a href="samp://', '">', $pos);
		$pos = strpos($page, '<a href="samp://', $pos + 1);
		fwrite($master, $ip."\n");
		$servers++;
	}
	$i += MASTERLIST_SERVERS_PER_PAGE;
}
fclose($master);

if($servers >= MIN_SERVERS_FOR_REWRITE) 
	copy(MASTERLIST_FILE_TEMP, MASTERLIST_FILE);
unlink(MASTERLIST_FILE_TEMP);

$exec_time = microtime(true)-$start_time;
echo $servers.' servers fetched in '.$exec_time. ' seconds.';

if(defined('LOG_EXECTIME_FILE')) 
{
	$log = fopen(LOG_EXECTIME_FILE, 'a');
	fwrite($log, '['.date('Y/m/d H:i:s').'] '.$servers.' servers fetched in '.$exec_time. ' seconds.'."\n");
	fclose($log);
}
?>
