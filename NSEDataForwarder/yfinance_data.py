from ntpath import isdir
import yfinance as yf
import os

class yf_data:
    def __init__(self, ticker, period, pathToSave, interval):
        self.ticker = ticker
        self.period = period
        self.pathToSave = pathToSave
        self.interval = interval

    def get_data(self):
        data = yf.Ticker(self.ticker)
        self.df = data.history(period=self.period, interval = self.interval) 
        
    def post_data(self):
        if os.path.isdir(self.pathToSave[:9]):
            self.df.to_csv(self.pathToSave, index = False)
        else:
            os.mkdir(self.pathToSave[:9])
            self.df.to_csv(self.pathToSave, index = False)
        

def main():
    ticker = "AAPL"
    period = "1d"
    interval = "1m"
    path = f"./dataset/yf_dataset_{ticker}.csv"
    dataset = yf_data(ticker, period, path, interval)
    dataset.get_data()
    dataset.post_data()


if __name__ == "__main__":
    main()

        