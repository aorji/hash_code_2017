#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>

//  g++ hash_code.cpp  && ./a.out && echo "file:"  && cat output

struct Video
{
    int idx;
    int request;
    int size;
    bool closed;

    bool operator<(const Video &r)
    {
        return request > r.request;
    }

};


struct Cache
{
    int idx;
    int capacity;
    std::vector<int> video_idx;
};

struct CacheConnetcion
{
    int latency;
    Cache *cache;

};

bool operator<(const CacheConnetcion &l, const CacheConnetcion &r)
{
    return l.latency > r.latency;
}

struct Endpoint
{
    size_t idx;
    int datacenter_latency;
    std::vector<Video> video_request;
    std::priority_queue<CacheConnetcion> conected_cache;

    bool operator<(const Endpoint &r)
    {
        return datacenter_latency > r.datacenter_latency;
    }
};

struct DataCenter
{
    /* data */
    std::vector<Endpoint> endpoints;
    std::vector<Cache*> caches;
    std::vector<int> video_size;

    void TestData(void)
    {
        int i = 0;
        for(auto ep: endpoints)
        {
            std::cout << "ep " << i << ":" << std::endl;
            std::cout << "dc_latency: " << ep.datacenter_latency << std::endl;
            std::cout << "video:" << std::endl;
            for(auto & video: ep.video_request)
            {
                std::cout << video.idx << ": " << video.request/video.size << std::endl;
            }
            i++;
        }
    }

    void Output(void)
    {
        int used_cash = 0;
        for(auto &cache: caches)
            if (cache->video_idx.size() > 0) used_cash++;

        std::ofstream fout("output");
        fout << used_cash << "\n";
        for(int i = 0; i < caches.size(); ++i)
        {
            std::cout << "caches " << i << ": " << std::endl;
            if (caches[i]->video_idx.size())
            {
                std::cout << "video idx: ";
                fout << i << " ";
                for(auto j = 0; j < caches[i]->video_idx.size(); ++j)
                {
                    if (j != 0)
                        fout << " ";
                    fout << caches[i]->video_idx[j];
                    std::cout << caches[i]->video_idx[j] << " ";
                }
                std::cout << std::endl;
                fout << "\n";
            }
        }
    }

    void Input(void)
    {
        int V, E, R, C, X;
        std::ifstream fin("input_1");
        fin >> V >> E >> R >> C >> X;

        video_size.resize(V);
        caches.resize(C);
        endpoints.resize(E);

        for(auto &cache: caches)
            cache = new Cache;

        //fill vodeo size
        for(int i = 0; i < V; ++i)
            fin >> video_size[i];

        //fill endpoint
        for(int i = 0; i < E; ++i)
        {
            int cache_num;
            endpoints[i].idx = i;
            fin >> endpoints[i].datacenter_latency >> cache_num;

            while(cache_num--)
            {
                int cache_idx, latency;
                fin >> cache_idx >> latency;
                caches[cache_idx]->capacity = X;
                endpoints[i].conected_cache.push({latency, caches[cache_idx] });
            }
        }

        //fill request
        while(R--)
        {
            int video_idx, ep_idx, req;
            fin >> video_idx >> ep_idx >> req;
            endpoints[ep_idx].video_request.push_back({video_idx, req, video_size[video_idx], false});
        }
    }

    /*processing*/
    void Arrange(void)
    {
        std::sort(endpoints.begin(), endpoints.end());

        //fill caches
        for(auto & ep: endpoints) //for all endpoints
        {
            std::sort(ep.video_request.begin(), ep.video_request.end());
            while(!ep.conected_cache.empty()) //till we have cache connected to the ep
            {
                auto conected_cache = ep.conected_cache.top().cache;  //get cache with less latency (top latency)
                for(int i = 0; i < ep.video_request.size(); ++i)   //go though all videos starting from the most profitamle
                {
                    auto video = ep.video_request[i];    //get most profitable video
                    if (video.closed == true) continue;     //if video request already satisfied - continue with next video
                    if (conected_cache->capacity >= video.size)    //if cache has enough space for a particular video
                    {
                        if (std::find(std::begin(conected_cache->video_idx), std::end(conected_cache->video_idx), video.idx) == std::end(conected_cache->video_idx)) //and not stored in cache yet
                        {
                            conected_cache->capacity -= video.size;    //update capacity
                            conected_cache->video_idx.push_back(video.idx);    //save video index
                        }
                        ep.video_request[i].closed = true;
                    }
                }
                ep.conected_cache.pop();   //del pointer to cache from ep's cache list
            }
        }
    }
};

int main()
{
    DataCenter data_center;

    data_center.Input();
    data_center.Arrange();
    data_center.Output();
}
