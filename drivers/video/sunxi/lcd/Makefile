obj-$(CONFIG_LCD_SUNXI) += lcd.o

lcd-objs := dev_lcd.o lcd_source_interface.o lcd_panel_cfg.o panels/panels.o panels/default_panel.o panels/tft720x1280.o\
            panels/vvx10f004b00.o panels/lp907qx.o panels/starry768x1024.o panels/sl698ph_720p.o panels/B116XAN03.o panels/lp079x01.o\
            panels/inet_dsi_panel.o panels/HX8394.o panels/inet_spi_panel.o
              
lcd-$(CONFIG_TV_GM7121) += panels/gm7121_cvbs.o