'''
Created on Oct 19, 2015

@author: wirkert
'''


from tabulate import tabulate  # do sudo pip install tabulate
import numpy as np
from sklearn.linear_model import LinearRegression
from sklearn.ensemble import RandomForestRegressor
from sklearn.grid_search import GridSearchCV
from sklearn.cross_validation import KFold
from sklearn.preprocessing import Normalizer
from sklearn.metrics import r2_score

from linear import LinearSaO2Unmixing
from domain_adaptation import estimate_logistic_regressor, resample

def get_data_from_batch(filenameprefix):
    """returns (X,y): X are absorptions represented by an array of shape
            [nr_samples x nr_wavelengths]. y an array of shape [nr_samples]
            representing the oxygenation values """
    reflectances = np.load(filenameprefix + "_reflectances.npy")
    saO2 = np.load(filenameprefix + "_saO2.npy")
    # add noise to reflectances
    noises = np.random.normal(loc=0., scale=0.15, size=reflectances.shape)
    reflectances += noises * reflectances
    reflectances = np.clip(reflectances, 0.00001, 1.)
    # normalize reflectances
    normalizer = Normalizer(norm='l1')
    reflectances = normalizer.transform(reflectances)
    # transform to absorption
    absorptions = -np.log(reflectances)
    return absorptions, saO2


if __name__ == '__main__':
    # first load training and testing data
    X_train, oxy_train = get_data_from_batch("/media/wirkert/data/Data/temp/" +
        "hard_train_2")
    X_gaussian_train, oxy_gaussian_train = get_data_from_batch("/media/wirkert/data/Data/temp/" +
        "gaussian_train_2")
    X_test, oxy_test = get_data_from_batch("/media/wirkert/data/Data/temp/" +
        "gaussian_test")

    # do domain adaptation
    lr = estimate_logistic_regressor(X_train, X_gaussian_train)
    weights = lr.predict_proba(X_train)[:, 1] / lr.predict_proba(X_train)[:, 0]

    # do standard linear unmixing
    linear_unmixing = LinearSaO2Unmixing()
    predicted_oxy_lu = linear_unmixing.predict(X_test)

    # learn linear regression from data
    X_train, oxy_train, weights = resample(X_train, oxy_train, weights)
    # do weighting of samples
    X_train_w = X_train  # * np.sqrt(weights)[:, None]
    oxy_train_w = oxy_train  # * np.sqrt(weights)
    linear_regression = LinearRegression(fit_intercept=False)
    linear_regression.fit(X_train, oxy_train_w)
    predicted_oxy_lr = linear_regression.predict(X_test)

    # use the random forest regressor
    kf = KFold(X_train.shape[0], 5, shuffle=True)
    param_grid = [
      {"n_estimators": np.array([50]),
       "max_depth": np.arange(5, 15, 2),
       "min_samples_leaf": np.array([5, 7, 10, 15])}]
    # train forest
    rf = GridSearchCV(RandomForestRegressor(50, max_depth=10, n_jobs=-1),
                      param_grid, cv=kf, n_jobs=-1)
    rf = RandomForestRegressor(50, max_depth=11, min_samples_leaf=5,
                                n_jobs=-1)
    rf.fit(X_train, oxy_train)
    predicted_oxy_rf = rf.predict(X_test)

    # nicely print each sample, its true value and its predicted values
    np.set_printoptions(precision=2)  # why doesn't this work?
    data = np.vstack((oxy_test,
                      predicted_oxy_lu, predicted_oxy_lr, predicted_oxy_rf)).T
    print "detailed results:"
    regressor_list = ["lin unmixing", "lin regression", "rf regression"]
    print tabulate(data, headers=["true value"] + regressor_list)

    print "\nr2 scores"
    print "lin unmixing, lin regression, rf regression"
    print r2_score(oxy_test, predicted_oxy_lu), \
                 linear_regression.score(X_test, oxy_test), \
                 rf.score(X_test, oxy_test)

    print "\nabs mean errors"
    f = lambda x: np.mean(np.abs(x - oxy_test))
    print "lin unmixing, lin regression, rf regression"
    print f(predicted_oxy_lu), f(predicted_oxy_lr), f(predicted_oxy_rf)

    print "\nabs error lower percentile"
    f = lambda x: np.percentile(np.abs(x - oxy_test), 25, axis=0)
    print "lin unmixing, lin regression, rf regression"
    print f(predicted_oxy_lu), f(predicted_oxy_lr), f(predicted_oxy_rf)

    print "\nabs error higher percentile"
    f = lambda x: np.percentile(np.abs(x - oxy_test), 75, axis=0)
    print "lin unmixing, lin regression, rf regression"
    print f(predicted_oxy_lu), f(predicted_oxy_lr), f(predicted_oxy_rf)


