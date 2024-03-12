library(ggplot2)
library(scales)


#Benchmark 1
debugdata <- read.csv("threads_benchmark_grayscaleconversion(debug).csv")
releasedata <- read.csv("threads_benchmark_grayscaleconversion(release).csv")

debugdata$type <- "debug"
releasedata$type <- "release"

combineddata <- rbind(debugdata, releasedata)

ggplot(data = combineddata, mapping = aes(x = number_of_threads, y = runtime_in_seconds, color = type)) +
  geom_point() +
  geom_line() +
  geom_smooth() +
  theme_minimal() +
  scale_x_continuous(breaks = 1:max(combineddata$number_of_threads)) +
  labs(title = "Runtime of parallelized grayscale conversion",
       x = "Number of Threads",
       y = "Runtime in Seconds",
       color = "Compilation mode") +
  expand_limits(y = 0)  # Ensure y-axis starts from 0

ggsave("parallelgrayscale_benchmark.png", device = "png", width = 1920, height = 1080, units = "px", dpi = 150, bg="white")



#Benchmark 2
debugdata <- read.csv("threads_benchmark_adaptivethresholding(debug).csv")
releasedata <- read.csv("threads_benchmark_adaptivethresholding(release).csv")

debugdata$type <- "debug"
releasedata$type <- "release"

combineddata <- rbind(debugdata, releasedata)

ggplot(data = combineddata, mapping = aes(x = number_of_threads, y = runtime_in_seconds, color = type)) +
  geom_point() +
  geom_line() +
  geom_smooth() +
  theme_minimal() +
  scale_x_continuous(breaks = 1:max(combineddata$number_of_threads)) +
  labs(title = "Runtime of parallelized adaptive thresholding + sequential grayscale conversion",
       x = "Number of Threads",
       y = "Runtime in Seconds",
       color = "Compilation mode") +
  expand_limits(y = 0)  # Ensure y-axis starts from 0

ggsave("paralleladaptive_sequentialgrayscale_benchmark.png", device = "png", width = 1920, height = 1080, units = "px", dpi = 150, bg="white")


#Benchmark 3
debugdata <- read.csv("threads_benchmark_adaptivethresholding(debug).csv")
releasedata <- read.csv("threads_benchmark_adaptivethresholding(release).csv")

debugdata$type <- "debug"
releasedata$type <- "release"

combineddata <- rbind(debugdata, releasedata)

ggplot(data = combineddata, mapping = aes(x = number_of_threads, y = runtime_in_seconds, color = type)) +
  geom_point() +
  geom_line() +
  geom_smooth() +
  theme_minimal() +
  scale_x_continuous(breaks = 1:max(combineddata$number_of_threads)) +
  labs(title = "Runtime of parallelized adaptive thresholding + parallelized grayscale conversion",
       x = "Number of Threads",
       y = "Runtime in Seconds",
       color = "Compilation mode") +
  expand_limits(y = 0)  # Ensure y-axis starts from 0

ggsave("paralleladaptive_parallelgrayscale_benchmark.png", device = "png", width = 1920, height = 1080, units = "px", dpi = 150, bg="white")