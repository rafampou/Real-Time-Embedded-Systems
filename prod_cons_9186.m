[A,delimiterOut]=importdata("data_9186.txt");

B=mean(A,2);
figure
hold on;
title('Line Plot of waiting time on queue')
ylabel('Waiting Time (ns)') 
xlabel('Number of Consumer. (Power of 2)') 
plot(B)
hold off;
figure
hold on;
title('boxplot of waiting time')
ylabel('Waiting Time (ns)') 
xlabel('Number of Consumer. (Power of 2)') 
boxplot(A')
