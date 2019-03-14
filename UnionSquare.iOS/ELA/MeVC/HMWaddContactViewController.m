//
//  HMWaddContactViewController.m
//  ELA
//
//  Created by 韩铭文 on 2019/1/5.
//  Copyright © 2019 HMW. All rights reserved.
//

#import "HMWaddContactViewController.h"
#import "HMWFMDBManager.h"
#import "friendsModel.h"
#import "ScanQRCodeViewController.h"

@interface HMWaddContactViewController ()
@property (weak, nonatomic) IBOutlet UITextField *nickNameTextField;
@property (weak, nonatomic) IBOutlet UITextField *theWalletAddressTextField;
@property (weak, nonatomic) IBOutlet UIButton *QrCodeButton;
@property (weak, nonatomic) IBOutlet UIButton *pasteButton;
@property (weak, nonatomic) IBOutlet UITextField *mobilePhoneNOTextField;
@property (weak, nonatomic) IBOutlet UITextField *emailTextField;
@property (weak, nonatomic) IBOutlet UITextField *noteTextField;
@property (weak, nonatomic) IBOutlet UIButton *addBuddyButton;

@end

@implementation HMWaddContactViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    [self defultWhite];
    [self setBackgroundImg:@"setting_bg"];
    self.nickNameTextField.placeholder=NSLocalizedString(@"请输入姓名（必填）", nil);
//    self.title=@"钱包列表";
    
    
    self.theWalletAddressTextField.placeholder=NSLocalizedString(@"请输入钱包地址（必填）", nil);
    self.mobilePhoneNOTextField.placeholder=NSLocalizedString(@"请输入手机号码", nil);
    self.emailTextField.placeholder=NSLocalizedString(@"请输入邮箱", nil);
   [[HMWCommView share]makeTextFieldPlaceHoTextColorWithTextField: self.nickNameTextField];
    [[HMWCommView share]makeTextFieldPlaceHoTextColorWithTextField:self.theWalletAddressTextField];
 
 
  [[HMWCommView share]makeTextFieldPlaceHoTextColorWithTextField:self.mobilePhoneNOTextField];
  [[HMWCommView share]makeTextFieldPlaceHoTextColorWithTextField:self.emailTextField];
  [[HMWCommView share]makeTextFieldPlaceHoTextColorWithTextField:self.noteTextField];
    [[HMWCommView share]makeBordersWithView:self.addBuddyButton];
    self.noteTextField.placeholder=NSLocalizedString(@"请输入备注", nil);
    [self.addBuddyButton setTitle:NSLocalizedString(@"添加", nil) forState:UIControlStateNormal];
    if (self.model.nameString.length>0) {
        
    
    
    self.nickNameTextField.text=self.model.nameString;
    
    self.theWalletAddressTextField.text=self.model.address;
    
    self.mobilePhoneNOTextField.text=self.model.mobilePhoneNo;
    self.emailTextField.text=self.model.email;
    
        self.noteTextField.text=self.model.note;
        
        [self.addBuddyButton setTitle:NSLocalizedString(@"确认修改", nil) forState:UIControlStateNormal];
    }
    
    
    
}
- (IBAction)sweepTheQRCodeEvent:(id)sender {

    __weak __typeof__(self) weakSelf = self;
    ScanQRCodeViewController *scanQRCodeVC = [[ScanQRCodeViewController alloc]init];
    scanQRCodeVC.scanBack = ^(NSString *addr) {
        
        weakSelf.theWalletAddressTextField.text=addr;
    
    };
    [self QRCodeScanVC:scanQRCodeVC];
}

- (IBAction)pasteTheEvent:(id)sender {
  self.theWalletAddressTextField.text = [[FLTools share]pastingTextFromTheClipboard];
    
}
- (IBAction)addBuddyEvent:(id)sender {
    if (self.nickNameTextField.text.length==0||self.theWalletAddressTextField.text==0) {
        return;
    }
    friendsModel *model=[[friendsModel alloc]init];
   
    model.nameString=self.nickNameTextField.text;
    model.address=self.theWalletAddressTextField.text;
    model.mobilePhoneNo=self.mobilePhoneNOTextField.text;
    model.email=self.emailTextField.text;
    model.note=self.noteTextField.text;
    
    if (self.model.nameString.length==0) {
        if ([[HMWFMDBManager sharedManagerType:friendsModelType]addRecord:model]){
            [[FLTools share]showErrorInfo:NSLocalizedString(@"添加成功！", nil)];
            [self.navigationController popViewControllerAnimated:YES];
        }
        
    }else{
        model.ID=self.model.ID;
        if ([[HMWFMDBManager sharedManagerType:friendsModelType]updateRecord:model]){
            [[FLTools share]showErrorInfo:NSLocalizedString(@"修改成功！", nil)];
            [self.navigationController popViewControllerAnimated:YES];
        }
        
    }
    
    
}
-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
    
    [self.view endEditing:YES];
    
}
-(void)setModel:(friendsModel *)model{
    _model=model;
    
}
@end
