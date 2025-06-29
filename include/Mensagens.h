#define STR_LEN 11 

struct aviator_msg { 
    int32_t   player_id;  
    float     value; 
    char      type[STR_LEN]; 
    float     player_profit; 
    float     house_profit; 
}; 