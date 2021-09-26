full_cnted= xlsread('C:\Users\juhsu\Desktop\DTW experiment\fully connected vect.xlsx');
smlwr_cnted10 = xlsread('C:\Users\juhsu\Desktop\DTW experiment\small world vec10.xlsx');
smlwr_cnted40 = xlsread('C:\Users\juhsu\Desktop\DTW experiment\small world vec40.xlsx');
smlwr_cnted70 = xlsread('C:\Users\juhsu\Desktop\DTW experiment\small world vec70.xlsx');
[dist,x,y] = dtw(full_cnted,smlwr_cnted);

% figure;
% mainbar=subplot('position',[0.19 0.19 0.67 0.79]);
%    plot(x,y);
%    set(gca,'ytick',[]);
%    set(gca,'xtick',[]);
%    set(gca,'ylim',[1 20]);
%    set(gca,'xlim',[1 100]);
   
leftbar = subplot('position',[0.07 0.19 0.10 0.79]);
   stairs(smlwr_cnted);
   set(gca,'xlim',[1 20]);
   set(gca,'ylim',[0 1]);
   set(gca,'ytick',[0:1]);
   view(-90,90); 
bottombar = subplot('position',[0.19 0.07 0.67 0.10]);
   stairs(full_cnted);
   set(gca,'xlim',[1 100]);
   set(gca,'ylim',[0 1]);
   set(gca,'ytick',[0:1]);