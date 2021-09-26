clear
clc
a=xlsread('C:\Users\juhsu\Desktop\DTW experiment\fully connected vect.xlsx')';
b10=xlsread('C:\Users\juhsu\Desktop\DTW experiment\small world vec10.xlsx')';
b40=xlsread('C:\Users\juhsu\Desktop\DTW experiment\small world vec40.xlsx')';
b70=xlsread('C:\Users\juhsu\Desktop\DTW experiment\small world vec70.xlsx')';
[Dist,D,k,w,rw,tw] = dtw(a,b10,1);
[Dist,D,k,w,rw,tw] = dtw(a,b40,1);
[Dist,D,k,w,rw,tw] = dtw(a,b70,1);