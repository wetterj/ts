library("ggplot2")
require(gridExtra)

d <- read.csv("./open-fails-data", sep=" ")
pltFails <- ggplot(d) + theme(text=element_text(family="Times"), legend.justification=c(0,1), legend.position=c(0,1)) +
       ggtitle(expression(paste("Failures During Search", ' '%*%' ', 10^{-8}))) +
       coord_fixed(1.5) +
       xlab("No Additional Propagation") + ylab("Additional Propagation") +
       #scale_x_continuous(limits=c(1,10e6)) + scale_y_continuous(limits=c(1,10e6)) +
       #scale_x_log10(limits=c(1,10e6)) + scale_y_log10(limits=c(1,10e6)) +
       geom_abline() + scale_shape_discrete(solid=F) + 
       geom_point(aes(x=base/1e8, y=upperBoundQR/1e8, shape="Upper bound qr", col="Upper bound qr"), size=4) + 
       geom_point(aes(x=base/1e8, y=upperBoundQ/1e8, shape="Upper bound q", col="Upper bound q"), size=4) + 
       geom_point(aes(x=base/1e8, y=upperBoundX/1e8, shape="Upper bound x", col="Upper bound x"), size=4) + 
       geom_point(aes(x=base/1e8, y=all/1e8, shape="All", col="All"), size=4) + 
       #scale_colour_manual(labels=c("qr" = expression(q[r]),"q"="q","x"=expression(x[{ri}]),"a"="All"),values=c("blue","red","green","yellow")) +
       guides(color=guide_legend("Configuration"), shape=guide_legend("Configuration"))

d <- read.csv("./open-time-data", sep=" ")
pltTime <- ggplot(d) + theme(text=element_text(family="Times"), legend.justification=c(0,1), legend.position=c(0,1)) +
       ggtitle("Search Execution Time") +
       coord_fixed(1.5) +
       xlab("No Additional Propagation") + ylab("Additional Propagation") +
       #scale_x_continuous(limits=c(1,10e6)) + scale_y_continuous(limits=c(1,10e6)) +
       #scale_x_log10(limits=c(1,10e6)) + scale_y_log10(limits=c(1,10e6)) +
       geom_abline() + scale_shape_discrete(solid=F) + 
       geom_point(aes(x=base, y=upperBoundQR, shape="Upper bound qr", col="Upper bound qr"), size=4) + 
       geom_point(aes(x=base, y=upperBoundQ, shape="Upper bound q", col="Upper bound q"), size=4) + 
       geom_point(aes(x=base, y=upperBoundX, shape="Upper bound x", col="Upper bound x"), size=4) + 
       geom_point(aes(x=base, y=all, shape="All", col="All"), size=4) + guides(color=guide_legend("Configuration"), shape=guide_legend("Configuration"))

ggsave("./open.pdf", arrangeGrob( pltFails, pltTime, ncol=2 ) ) 
