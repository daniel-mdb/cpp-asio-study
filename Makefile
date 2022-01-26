CXX_FLAGS += -Wall -Wextra
CXX_FLAGS += -g
INCLUDES += -Iasio/asio/include
LIBS += -lpthread

all: throttling_proxy_simple throttling_proxy_size throttling_proxy_speed 2
2: throttling_proxy2_simple throttling_proxy2_speed throttling_proxy2_size

throttling_proxy_*: CXX_FLAGS += --std=c++20
throttling_proxy2_*: CXX_FLAGS += --std=c++20
throttling_proxy2_*: CXX_FLAGS += -DASIO_ENABLE_BUFFER_DEBUGGING=1 # -fsanitize=address
throttling_proxy2_*: CXX_FLAGS += -DASIO_ENABLE_HANDLER_TRACKING=1

throttling_proxy_simple: throttling_proxy.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -O0 -o $@ $<

throttling_proxy_size: throttling_proxy.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -Os -o $@ $<

throttling_proxy_speed: throttling_proxy.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -O3 -o $@ $<

throttling_proxy2_simple: throttling_proxy2.cc
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -O0 -o $@ $<

throttling_proxy2_size: throttling_proxy2.cc
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -Os -o $@ $<

throttling_proxy2_speed: throttling_proxy2.cc
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -O3 -o $@ $<
