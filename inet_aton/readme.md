#2021/03/09

inet_aton与之前的inet_addr并无本质不同，但是inet_aton使用了in_addr(sockaddr_in)结构体，实际用的更多。

![avatar](sockaddr_in.jpg)