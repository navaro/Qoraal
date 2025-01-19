echo "this is a script to show the demo commands"
sleep 1000
ctrl system stop
echo
echo
echo "list available commands"
?
sleep 1000
echo
echo
echo "start the demo service"
ctrl demo start
echo
echo
echo "list commands installed by demo service"
? demo
sleep 1000
echo
echo
echo "run the 'demo_tasks' command"
demo_tasks