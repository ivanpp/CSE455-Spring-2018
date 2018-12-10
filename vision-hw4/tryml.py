from uwimg import *

def softmax_model(inputs, outputs):
    l = [make_layer(inputs, outputs, SOFTMAX)]
    return make_model(l)

def neural_net(inputs, outputs):
    print inputs
    l = [   make_layer(inputs, 32, LRELU),
            make_layer(32, outputs, SOFTMAX)]
    return make_model(l)

def neural_net_three(inputs, outputs):
    l = [   make_layer(inputs, 64, LRELU),
            make_layer(64, 32, LRELU),
            make_layer(32, outputs, SOFTMAX)]
    return make_model(l)

print("loading data...")
#train = load_classification_data("mnist.train", "mnist.labels", 1)
#test  = load_classification_data("mnist.test", "mnist.labels", 1)
train = load_classification_data("cifar.train", "cifar/labels.txt", 1)
test = load_classification_data("cifar.test", "cifar/labels.txt", 1)
print("done")
print

print("training model...")

""" 2.2 softmax_model
batch = 128
iters = 1000
rate = .1
momentum = .9
decay = .0

m = softmax_model(train.X.cols, train.y.cols)
"""

""" 2.3 neural_net
batch = 128
iters = 10000
rate = .1
momentum = .9
decay = .01

m = neural_net(train.X.cols, train.y.cols)
"""

""" 2.3.4 neural_net(3 layers)
batch = 128
iters = 3000
rate = .1
momentum = .9
decay = .01

m = neural_net_three(train.X.cols, train.y.cols)
"""

#""" 3.1 training on CIFAR
batch = 128
iters = 3000
rate = .01
momentum = .9
decay = .01

m = neural_net_three(train.X.cols, train.y.cols)
#"""

train_model(m, train, batch, iters, rate, momentum, decay)
print("done")
print

print("evaluating model...")
print("training accuracy: %f", accuracy_model(m, train))
print("test accuracy:     %f", accuracy_model(m, test))


## Questions ##

# 2.2.1 Why might we be interested in both training accuracy and testing accuracy? What do these two numbers tell us about our current model?
# Training accuracy tells us how well our model fits the training data currently,
# while testing accuracy tells us how well our model generalizes to unseen data.

# 2.2.2 Try varying the model parameter for learning rate to different powers of 10 (i.e. 10^1, 10^0, 10^-1, 10^-2, 10^-3) and training the model. What patterns do you see and how does the choice of learning rate affect both the loss during training and the final model accuracy?
# In this case:
# 10^1 seems too large, prediction scores goes too large which causes prediction probability goes to NAN, so does the loss. It breaks down(using C interface). And using the python interface(tryml.py),
# it gets a pretty bad model with about 25% training/test accuracy but does not crash(This may because python automatically deal with the NAN things while converting things from ctype).
# 10^0 seems a bit large, the model converges very fast, then the loss goes up and down. It just jump around the local/global minima.
# 10^-1 seems to be the best order of magnitude, the models converges fast and gets a good training/test accuracy.
# 10^-2 seems to be appropriate too and the convergence is a litter slower.
# 10^-3 seems to be inappropriate, the converges is too slow.

# 2.2.3 Try varying the parameter for weight decay to different powers of 10: (10^0, 10^-1, 10^-2, 10^-3, 10^-4, 10^-5). How does weight decay affect the final model training and test accuracy?
# In this case, 10^-2 is an appropriate choice. But it turns out that decay that less than 10^-2 shows no difference.
# This may result from the same order of magnitude between capacity of the model and the dim of the data.
# If the decay is too large(e.g. 10^1 in this case), the training/test accuracy hurts. Excessive regularization causes underfitting.
# If the decay is too small(or no decay) meanwhile the model is too powerful(not in this case), there will be a gap between test and training accuracy due to the overfitting.

# 2.3.1 Currently the model uses a logistic activation for the first layer. Try using a the different activation functions we programmed. How well do they perform? What's best?
# Remain batch=128, iters=1000, rate=.1, momentum=.9, decay=0
# activation | training accuracy | test accuracy |
# LOGISTIC   | 0.943283          | 0.941400      |
# RELU       | 0.950750          | 0.943800      |
# LRELU      | 0.953167          | 0.945900      |
# Leaky relu is the best default choice in this case(and in many other cases actually).

# 2.3.2 Using the same activation, find the best (power of 10) learning rate for your model. What is the training accuracy and testing accuracy?
# Remain batch=128, momentum=.9, decay=0, and use LRELU activation.
# To avoid lager learning rate takes advantage of the faster convergence,
# iters is fixed to a relatively large number, 10000.
# learning rate | training accuracy | test accuracy |
# 10^0          | 0.981600          | 0.960100      | (crashed in C)
# 10^-1         | 0.983500          | 0.962200      |
# 10^-2         | 0.971733          | 0.960800      |
# 10^-3         | 0.924450          | 0.925600      |
# The best learning rate magnitude is about 10^-1 in this case.

# 2.3.3 Right now the regularization parameter `decay` is set to 0. Try adding some decay to your model. What happens, does it help? Why or why not may this be?
# Remain batch=128, iters=10^4, rate=.1, momentum=.9, and use LRELU activation.
# decay | training accuracy | test accuracy | gap      |
# 10^0  | 0.931900          | 0.933400      | 0.001500 |
# 10^-1 | 0.970300          | 0.965000      | 0.005300 |
# 10^-2 | 0.980350          | 0.963700      | 0.016650 |
# 0     | 0.983500          | 0.962200      | 0.021300 |
# It does help. While decay gets larger, the gap between training and test accuracy becomes smaller.
# And the training accuracy improves when appropriate decay rate has been chosen.
# Weight decay(a.k.a. L2 regularization) penalizes more while weights are larger.
# So if we choose an appropriate rate for weight decay,
# we can get a model that can generlize new unseen data well and learn well from training dataset at the same time.

# 2.3.4 Modify your model so it has 3 layers instead of two. The layers should be `inputs -> 64`, `64 -> 32`, and `32 -> outputs`. Also modify your model to train for 3000 iterations instead of 1000. Look at the training and testing error for different values of decay (powers of 10, 10^-4 -> 10^0). Which is best? Why?
# Structure: LRELU - LRELU - SOFTMAX, iters=3000
# decay | training accuracy | test accuracy | gap      |
# 10^0  | 0.932367          | 0.930500      | 0.001867 |
# 10^-1 | 0.966267          | 0.961100      | 0.005167 |
# 10^-2 | 0.980383          | 0.968700      | 0.011683 |
# 10^-3 | 0.977383          | 0.961700      | 0.015683 |
# 10^-4 | 0.979517          | 0.966000      | 0.013517 |
# 0     | 0.979717          | 0.966200      | 0.013517 |
# Decay rate around 10^-2 gets the best model in this case, perform better than no decay and high decay rates.
# Cuz the 3-layer-network is powerful enough for this mnist task.
# If no decay at all(or too samll rate), the model will fit the training dataset too well which leads to bad performance on new unseen data.(high training acc, low test acc)
# Also, too large decay rate will force our model to be simple and may not be powerful enough to deal with our task.(low training acc, low test acc)

# 3.2.1 How well does your network perform on the CIFAR dataset?
# Same network structure on CIFAR dataset.
# Reamin batch=128, iters=3000, momentum=.9.
# Tuning the learning rate and decay rate roughly(just vary them to different powers of 10)
# rate=10^-2, decay=10^-2 gives us the best model(roughly) in this case,
# with a 0.4503 training accuracy and 0.4371 test accuracy.
