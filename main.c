#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_MAX_WEIGHTS 100
#define N_MAX_NODES 100
#define N_MAX_LAYERS 100
#define WEIGHT_RANGE 10
#define WEIGHT_OFFSET 0

struct Node
{
    float weights[N_MAX_WEIGHTS];
    int n_weights;
    float output;
};

struct Layer
{
    struct Node *nodes[N_MAX_NODES];
    int n_nodes;
};

struct Network
{
    struct Layer *layers[N_MAX_LAYERS];
    int n_layers;
    int n_inputs;
    int major_n_nodes;
    int n_outputs;
};

float randn(int range, float offset) {
    return ((float)rand() / (float)RAND_MAX) * range + offset;
}

struct Node *new_node(int n_weights)
{
    struct Node *node = malloc(sizeof(struct Node));
    node->n_weights = n_weights;
    node->output = 0.0f;

    for (int i = 0; i < n_weights; i++) {
        *(node->weights + i) = randn(WEIGHT_RANGE, WEIGHT_OFFSET);
    }

    return node;
}

struct Layer *new_layer(int n_nodes, int n_inputs)
{
    struct Layer *layer = malloc(sizeof(struct Layer));
    layer->n_nodes = n_nodes;

    for (int i = 0; i < n_nodes; i++) {
        *(layer->nodes + i) = new_node(n_inputs);
    }

    return layer;
}

struct Network *new_network(int n_inputs, int n_outputs, int *n_nodes_per_layer, int n_internal_layers)
{
    struct Network *network = malloc(sizeof(struct Network));
    network->n_layers = n_internal_layers + 1;
    network->n_inputs = n_inputs;
    network->n_outputs = n_outputs;

    int n_inputs_of_layer = n_inputs;

    int i;

    network->major_n_nodes = 0;

    for (i = 0; i < n_internal_layers; i++) {
        *(network->layers + i) = new_layer(n_nodes_per_layer[i], n_inputs_of_layer);
        n_inputs_of_layer = n_nodes_per_layer[i];

        if (n_nodes_per_layer[i] > network->major_n_nodes) {
            network->major_n_nodes = n_nodes_per_layer[i];
        }
    }

    *(network->layers + i) = new_layer(n_outputs, n_inputs_of_layer);

    return network;
}

void network_info(struct Network *network) {
    printf("\n--- Network Info ---\n");
    printf("N inputs: %d\n", network->n_inputs);
    printf("N internal layers: %d\n", network->n_layers - 1);
    printf("N outputs: %d\n", network->n_outputs);
    printf("Major n layers: %d\n", network->major_n_nodes);
    printf("\n");
    printf("- Internal Layers\n");
    struct Layer *aux_layer;
    struct Node *aux_node;
    int i;
    for (i = 0; i < network->n_layers - 1; i++) {
        aux_layer = *(network->layers + i);
        printf("Layer #%d: %d nodes\n", i+1, aux_layer->n_nodes);
        for (int j = 0; j < aux_layer->n_nodes; j++) {
            aux_node = *(aux_layer->nodes + j);
            printf("Node #%d: ", j+1);
            for (int k = 0; k < aux_node->n_weights; k++) {
                printf("%.2f ", aux_node->weights[k]);
            }
            printf("\n");
        }
        printf("\n");
    }
    aux_layer = *(network->layers + network->n_layers - 1);
    printf("Output layer: %d nodes\n", aux_layer->n_nodes);
    aux_layer = *(network->layers + i);
    for (int j = 0; j < aux_layer->n_nodes; j++) {
        aux_node = *(aux_layer->nodes + j);
        printf("Node #%d: ", j+1);
        for (int k = 0; k < aux_node->n_weights; k++) {
            printf("%.2f ", aux_node->weights[k]);
        }
        printf("\n");
    }
}

float relu(float input)
{
    if (input < 0)
    {
        return 0.0f;
    }

    return input;
}

void eval_node(struct Node *node, float *inputs) {
    float acc = 0;

    for (int i = 0; i < node->n_weights; i++) {
        acc += node->weights[i] * inputs[i];
    }

    node->output = relu(acc);
}

float eval_layer(struct Layer *layer, float *inputs)
{
    struct Node *aux_node;
    for (int i = 0; i < layer->n_nodes; i++) {
        aux_node = *(layer->nodes + i);
        eval_node(aux_node, inputs);
    }
}

float *new_dinputs(struct Network *network) {
    float *inputs = malloc(sizeof(float) * network->major_n_nodes);

    return inputs;
}

float eval_network(struct Network *network, float *inputs) {
    struct Layer *aux_layer;
    struct Node *aux_node;
    float *dinputs = new_dinputs(network);

    for (int i = 0; i < network->n_inputs; i++) {
        dinputs[i] = inputs[i];
    }

    for (int i = 0; i < network->n_layers; i++) {
        aux_layer = *(network->layers + i);
        eval_layer(aux_layer, dinputs);

        for (int j = 0; j < aux_layer->n_nodes; j++) {
            aux_node = *(aux_layer->nodes + j);

            *(dinputs + j) = aux_node->output;
        }
    }

    printf("\n* Output *\n");
    for (int i = 0; i < aux_layer->n_nodes; i++) {
        aux_node = *(aux_layer->nodes + i);
        printf("Node #%d: %.2f\n", i+1, aux_node->output);
    }
}


int main()
{
    srand(time(NULL));
    int n_inputs = 3;
    int n_outputs = 1;
    int n_internal_layers = 4;
    int nodes_per_layer[4] = { 8, 6, 4, 4 };
    struct Network *net = new_network(n_inputs, n_outputs, nodes_per_layer, n_internal_layers);

    network_info(net);

    float inputs[net->n_inputs];
    inputs[0] = 1.0f;
    inputs[1] = 1.0f;
    inputs[2] = 1.0f;

    eval_network(net, inputs);

    return 0;
}
