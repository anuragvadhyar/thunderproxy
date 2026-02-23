#include "loadbalancer.h"

int main()
{
    try
    {
        LoadBalancer l{80, "config/loadbalancer.json"};
        l.configure();
        l.start_load_balancer();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    
    return 0;
}