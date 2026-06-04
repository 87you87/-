package com.expense.servlet;

public class ExpenseRecord {
    private String date;
    private String type;
    private String category;
    private double amount;
    private String note;
    
    public ExpenseRecord(String date, String type, String category, double amount, String note) {
        this.date = date;
        this.type = type;
        this.category = category;
        this.amount = amount;
        this.note = note;
    }
    
    public String getDate() {
        return date;
    }
    
    public String getType() {
        return type;
    }
    
    public String getCategory() {
        return category;
    }
    
    public double getAmount() {
        return amount;
    }
    
    public String getNote() {
        return note;
    }
    
    @Override
    public String toString() {
        return "ExpenseRecord{" +
                "date='" + date + '\'' +
                ", type='" + type + '\'' +
                ", category='" + category + '\'' +
                ", amount=" + amount +
                ", note='" + note + '\'' +
                '}';
    }
}
