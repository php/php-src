@if exist ..\ZendEngine2\OBJECTS2_HOWTO (
move ..\Zend ..\ZendEngine1
move ..\ZendEngine2 ..\Zend 
echo "PLEASE RESTART VISUAL C++ TO RELOAD THE ZEND PROJECT."
exit 1  )
