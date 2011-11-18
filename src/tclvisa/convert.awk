{
	if ( $2 != "" ) {
		print "#ifdef", $2
		print "addIntVar(", $2, ");"
		print "#endif\n";
	}
}

