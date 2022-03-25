function createfigure(X1, Y1)
%CREATEFIGURE(X1, Y1)
%  X1:  vector of x data
%  Y1:  vector of y data

%  Auto-generated by MATLAB on 01-Nov-2020 09:07:35

% Create figure
figure1 = figure;

% Create axes
axes1 = axes('Parent',figure1);
hold(axes1,'on');

% Create plot
plot(X1,Y1,'DisplayName','\delta_{COL} Analytical','LineWidth',2,...
    'LineStyle','--',...
    'Color',[0 0 0]);

% Create xlabel
xlabel('Distance [m]');

% Create ylabel
ylabel('Error probability');

% Uncomment the following line to preserve the Y-limits of the axes
% ylim(axes1,[0 1]);
box(axes1,'on');
grid(axes1,'on');
% Create legend
legend1 = legend(axes1,'show');
set(legend1,'Location','northwest');
