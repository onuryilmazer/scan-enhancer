library(ggplot2)
library(scales)

debugdata <- read.csv("threads_benchmark_debug.csv")
releasedata <- read.csv("threads_benchmark_release.csv")

debugdata$type <- "debug"
releasedata$type <- "release"

combineddata <- rbind(debugdata, releasedata)

ggplot(data = combineddata, mapping = aes(x = number_of_threads, y = runtime_in_seconds, color = type)) +
  geom_point() +
  geom_line() +
  geom_smooth() +
  theme_minimal() +
  scale_x_continuous(breaks = 1:max(combineddata$number_of_threads)) +
  labs(title = "Benchmark performance of 'Enhancer' based on the nr of threads",
       x = "Number of Threads",
       y = "Runtime in Seconds",
       color = "Compilation mode")

ggsave("threads_benchmark_combined.png", device = "png", width = 1920, height = 1080, units = "px", dpi = 150, bg="white")
