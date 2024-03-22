# Owl v0.5a  (\\^(OvO)^/)

Owl is an interpreted, dynamically typed, lexically scoped procedural language. With full support for recursive 
procedures implementing you're favorite algorithms is a breeze, while user defined records and dynamically typed arrays 
facilitate powerful abstractions.

Owl's Mascot (\\^(OvO)^/) is named Flaco. Named in dedication to the memory of Flaco the Owl who came
to fame for escaping his enclosure in New York City's Central Park Zoo after years of captivity. Flaco 
embodies a spirit of freedom from which we can all find some inspiration.


# Using Owl

To build owl run the included Makefile by typing 'make' in your terminal and running 'make install' as root. 
Owl requires a C++ compiler which supports C++17 to compile, and has been tested on linux 
and macOS with GNU GCC's G++ and the LLVM clang++ compilers. 

Once built, Owl can be invoked in two different ways:
1) Owl cli - your everyday command line interpreter, owl <options> <filename> will run a supplied owl program
2) Owl repl - a standard read-eval-print-loop. Enter statements and they run as soon as you hit enter, invoked with 


# Syntax

This is still very much in development, and as such is subject to change.
this should be viewed as no more than an approximation of Owl's grammar

## reserved words: 

      begin, end, input, print, if, then , else, while, 
      func, let, int, string, real, record, rand, program, library,
      return, ref, import, make

## reserved symbols:
      , . := : ; + - * / [ ] ( ) {* *} # < > <= >= == != 

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

      declareVar := let term <;> | { (<:=> term)* <;>}

      declareProcedure := func id ( {paraneter_list} ) block

      parameter_list := id<:> { ref } type {<,> parameter_list}*

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

      var := id[expression] | id.id | procedureCall

      num := ([0..9])*

      real_num := ([0..9])*<.>([0..9])*

      rand_num := [implementation dependent, returns random number]

      string_literal := <"> string <">

      Types := Integer | Real | String | Record
	  
	  multiline comment := {* comment
	  						  on multiple lines *}
      
	  single line comment := # comment on single line

##  Owl by Example

The 'owlcode' folder contains many examples outlining owl's different features and capabilities. 

    program 'fibonacci';
    begin
    	let m := 1;
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

    
# Misc
Owl is (c) 2024 Max Goren, http://maxgcoding.com, see LICENSE.md for more info.
