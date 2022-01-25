CXX_FLAGS += --std=c++20
CXX_FLAGS += -Wall -Wextra
CXX_FLAGS += -g
INCLUDES += -Iasio/asio/include
LIBS += -lpthread

all: throttling_proxy_simple throttling_proxy_size throttling_proxy_speed

throttling_proxy_simple: throttling_proxy.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -O0 -o $@ $<

throttling_proxy_size: throttling_proxy.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -Os -o $@ $<

throttling_proxy_speed: throttling_proxy.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -O3 -o $@ $<
