# Owl lang. v0.1a  (\\^(OvO)^/)

Owl is an interpreted, dynamically typed, procedural language.
It features pascal-like syntax, with C-style scope rules.
Variables and procedures are declare before used. Variables must be
declared with an _initial_ type, but a variables type can be changed 
implicitly through assignment

Owl's Mascot, (\\^(OvO)^/), is named Flaco, in dedication to the memory of Flaco the Owl who came
to fame for escaping his enclosure in the central park zoo after years of captivity.

# Using Owl

Owl can be invoked in three different ways:
   1) Owl shell - this interactive shell lets you load files, run them , display their AST, see memory usage etc
   2) Owl repl - a standard read-eval-print-loop, lets you enter statements, and they run as soon as you hit enter
   3) Owl cli - your normal CLI interpreter, owlcli <options> <filename> will run a supplied owl program

It is planned to merge owlshell and owlrepl sometime soon.

# Syntax

This is still very much in development, and as such is subject to change.
this should be viewed as no more than an approximation of Owl's grammar

## reserved words: 

      begin, end, input, print, if, then , else, while, 
      func, let, int, string, real, rand, program, library,
      return, ref, import

## reserved symbols:
      , . := : ; + - * / [ ] ( ) {* *} < > <= >= == !=

## Grammar

      program := program 'id';
      		{ import library_name; }
		 block
      
      library := library 'id';
                 begin
  			 declareProcedure { declareProcedure* }
		 end;
   
      block := begin 
               statement_list
               end

      statement_list := statement | statement*

      statement :=   (   
	      	          declareVar |
	             declareProcedure |
	               printStatement |
	               inputStatement |
                   whileStatment |
                      ifStatment |
             expressionStatement |
                return statement 
                    ) <;>

      declareVar := let term <:> type {<;> |  <:=> term <;>}

      declareProcedure := func id ( {paraneter_list} ) block

      parameter_list := id <:> type {<,> parameter_list}*

      expressionStatement := simpleExpression | assignmentStatement | procedureCall

      procedureCall := id ( argument_list )  

      argument_list := expression { <,> expression }*

      assignStatement := id <:=> expression

      whileStatement := while (expression) block

      ifStatement := if (expression) then
                        statement_list
                     {else 
                        statement_list}
                     end
      
      returnStatement := return expression

      expression := simpleExpression ( < | > | <= | >= | == | != ) simpleExpression        

      simpleExpression := term { (+|-) term }*

      term := factor { (*|/) factor }*

      factor := var | num | rand_num | real_num | string_literal | ( expression )

      var := id[expression] | procedureCall

      num := ([0..9])*

      real_num := ([0..9])*<.>([0..9])*

      rand_num := [implementation dependent, returns random number]

      string_literal := <"> string <">

      Types := Integer | Real | String

## Some Quirks

using the + operator on any type and a string will 
result in the cocatenation of the string and the string representation of the other value.

during all math op and rel ops, 
all values are cast to float, and then 
cast back to their appropriate type. *except strings.

right now, input will only accept integers. sorry.

##  Owl by Example

Recursive Procedures are fully supported making the canonical fibonacci algorithm straight forward to implement
While typing is dynamic, its still required to declare an inital type for the variable.

    program 'fibonacci';
    begin
    	let m: int := 1;
    	func fibR(n: int) begin
        	if (n < 2) then
            		return n;
             	else
            		return fibR(n - 1) + fibR(n-2);
             	end;
    	end
    	while (m <= 13) begin
             	print (fibR(m) + " ");
             	m := m + 1;
    	end;
    	print "\n";
    end

If recusrion isn't you're thing, you can owl use the while loop for iteration

    program 'fibonacciEx';
    begin
    	let prev: int := 1;
    	let next: int := 2;
    	let curr: int := 0;
    	let x: int := 1;
    
    	func calcAndPrint() begin
        	next := (prev + curr);
        	print (next + " ");
        	prev := curr;
        	curr := next;
    	end

    	while (x <= 10) begin
        	calcAndPrint();
        	x := (x + 1);
    	end;
    
    	print "\n";
    end

And you can always save yourself a few cycles by cacheing using Owl's Array type

    program 'fibonacci';
    begin
   	let m: int := 1;
    	let cache[15]: int;
    	func fibCached(n: int) begin
        
	 	if (n < 2) then
            		return n;
        	else
            		if (cache[n] != 0) then
                		return cache[n];
            		else
                		cache[n] := fibCached(n - 1) + fibCached(n-2);
            		end;
            		print (n + " ");
            		return cache[n];
        	end;
	 
    	end
        while (m <= 13) begin
             	print (fibR(m) + " ");
             	m := m + 1;
    	end;
    	print "\n";
    end

And if you find yourself using your fibonacci procedure in many different programs, you can
package it as a library, and import it into your code, reducing code duplication and encouraging re-use

    library 'fibLib';
    begin
        func fibR(n: int) begin 
        	if (n < 2) then
            		return n;
        	else
            		return fibR(n-1) + fibR(n-2);
        	end;
   	end;
    end
    
    program 'libtest';
    import fibLib;
    begin
         print (fibR(13) + "\n");
    end;
    
# Misc
Owl is (c) 2024 Max Goren, http://maxgcoding.com, see LICENSE.md for more info.
