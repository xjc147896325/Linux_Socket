#2021/03/12

这个学了time-wait这个概念，同时验证了为啥服务器ctrl+C后再重启时会在bind阶段报错，因为该端口在time-wait阶段，同时也可以配置可选项将其关闭。

注意该章节里面有个nagle算法，在传输大文件时应关闭。其对解决网络堵塞有帮助，原理是等待上一次的ack来后再进行下一次的发送，之间的时间缓存待发送数据。禁用后不会等待，但在大文件中数据流很大，无需等待，反而浪费时间。但当数据流发送间隔长且小时，nagle算法很有效。