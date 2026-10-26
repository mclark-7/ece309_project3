# Project 3 Design Log
**Author:** Matt Clark · **P2 code used:** instructor reference
**Tools used:** ChatGPT for help debugging, and creating test


## 1. Hash table
For the hash table, instead of open addressing, I used separate chaining. I used multiple vectors to create buckets that then store the tools. I chose this method because it makes more sense to me to handle collisions when two tools are connected to the same bucket. I used the FNV-1a hash function to find out which bucket each tool should go into. I also doubled the number of buckets whenever the load factor was greater than 0.75, which helps make searching a lot more efficient since the buckets are not as full. I tested the registry with 10,000 tools and got 16,384 buckets, which had a load factor of 0.610352. This resulted in the longest chain being 5. Looking at the numbers, it makes sense because there were a lot more buckets, but the chains were not nearly as long as if we had fewer buckets. I also compared FNV-1a to a weaker hash function using 10,000 tools. This resulted in FNV-1a having a longest chain of 6 and the weaker hash having a longest chain of 9,000. Adding tools is usually quick because the hash table only needs to resize every so often.

## 2. Pointer stability
For pointer stability, I used unique_ptr to store the tools in the buckets. When the hash table is too full, it needs to resize and move the pointers into new buckets. Even though the pointers are moved, the actual tools stay in the same location in memory, which is important because when a pointer is returned to a tool, it will still point to the correct tool even after the hash table gets resized.

## 3. Calculator trace
For the calculator, I used two stacks, one to hold the operators and one to hold the numbers. When calculating the expression 2*(3+4)-5, the calculator first puts the two into the value stack and puts the multiplication operator into a different stack. It stores 3 and 4 in the value stack and the addition operator in the other stack. When the parentheses are closed, it calculates 3+4 to get 7. Using PEMDAS, it multiplies 2 by 7. After that, it subtracts 5 from 14 to get 9. The two stacks help the calculator keep track of the numbers and operators while keeping the correct order.

## 4. Errors as values
For handling the errors, I used return values instead of throwing exceptions. When a calculator tries to divide by 0, it returns false and provides an error message instead of crashing the program. The workspace also returns false if someone tries to access something outside of the workspace. The tools can also return an error message when something doesn't work. Exceptions are usually good for problems that are unexpected.

## 5. What I would design differently
Before starting the code, I wish I understood what functions were actually being involved because I ran into a lot of problems when I was trying to compile the project, and I had to fix many different parts of the code that could have been avoided. I also should have tested each function more often because I would do a couple and then test them, which did not turn out to be more efficient than doing one at a time. The time I spent on finding the mistakes and fixing each function could have been avoided.