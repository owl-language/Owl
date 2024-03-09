# Owl lang. v0.1a  (\\^(OvO)^/)

Owl is a dynamically typed, algol-like, procedural language.
It features pascal-like syntax, with C-style scope rules.
Variables and procedures are declare before used. Variables must be
declared with an _initial_ type, but a variables type can be changed 
implicitly through assignment

Owl's Mascot, (\^(OvO)^/), is named Flaco, in dedication to the memory of, Flaco the Owl, who came
to fame for escaping his enclosure in the central park zoo after years of captivity.

# Syntax

This is still very much in development, and as such is subject to change.
this should be viewed as no more than an approximation of Owl's grammar

## reserved words: 

      begin, end, input, print, if, then , else, while, 
      func, let, int, string, real, rand, program, return

## reserved symbols:
      , . := : ; + - * / [ ] ( ) {* *} < > <= >= == !=

## Grammar

      program := block
    
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
This is the ONLY implicit type conversion in owl.

right now, input will only accept integers. sorry.

##  Owl by Example

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


    program 'bubblesort';
    begin
       let c: int := 1;
       let d: int := 2;
       let ds: int := 0;
       let i: int := 1;
       let x[10]: int;

       func swap(v: int, u: int) begin
          let t: int := 0;
             t := x[v];
          x[v] := x[u];
          x[u] := t;
          ds := 1;
       end

       func showArr(n: int) begin
           let k: int := 1;
          while (k < n) begin
            print (x[k] + " ");
            k := k + 1;
          end;
          print "\n";
       end

       func sortPass(m: int, n: int) begin
          c := m;
          d := c + 1;
          while (d < n) begin
             if (x[d] < x[c]) then
                swap(c, d);
             end;
             c := c + 1;
             d := c + 1;
          end;
       end

       func sort(l: int) begin
          ds := 0; {* did swap flag, once sortPass completes without setting to 1, were done *}
          sortPass(1, l);  {* do bubble pass *}
          end;
          if (ds = 0) then
            return 0;
          else
            sort(l);
          end;
       end

      {* main program starts here *}
      
       i := 1;
       while (i < 10) begin      
          x[i] := rand(100);
          i := i + 1;
       end;
      print "Random Numbers: ";
      showArr(10);
      sort(10);
      print "Sorted Numbers: ":
      showArr(10);
    end

# Misc
Owl is Releassed under the MIT License and is (c) 2024 Max Goren, http://maxgcoding.com
