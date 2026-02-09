# Project 5 Append-Only Data Log Questions and Answers

## Question 1: What could go wrong if we didn't lock the header?
## Answer:     if the header was not locked, the offset and count could be overwritten with incorrect values, 
##             and then the child processes could potentially attempt to write whatever record they are working on,
##             to the same record slot that another process was attempting to write to, overwriting values.

## Question 2: What could go wrong if we didn't lock the record?
## Answer:     If a second program was attempting to read from the file as this program was still appending the file, and there were no locks,
##             The second program could get incomplete data due to the records still being written as the second program was trying to read.
